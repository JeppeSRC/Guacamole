/*
MIT License

Copyright (c) 2022 Jesper

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Guacamole.h>
#include <Guacamole/util/util.h>

#include <shaderc/shaderc.hpp>
#include "shader.h"
#include "context.h"


namespace Guacamole {

static shaderc_shader_kind ShaderStageToShaderC(ShaderStage stage) {
    switch (stage) {
        case ShaderStage::Vertex:
            return shaderc_vertex_shader;
        case ShaderStage::Fragment:
            return shaderc_fragment_shader;
        case ShaderStage::Geometry:
            return shaderc_geometry_shader;
        case ShaderStage::Compute:
            return shaderc_compute_shader;
    }

    GM_VERIFY(false);

    return shaderc_glsl_infer_from_source; // Should never be reached
}

static VkShaderStageFlags ShaderStageToVkShaderStage(ShaderStage stage) {
    switch (stage) {
        case ShaderStage::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::Geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
    }

    GM_VERIFY(false);

    return VK_SHADER_STAGE_ALL_GRAPHICS; // Should never be reached
}

Shader::ShaderModule::ShaderModule(const std::string& file, bool src, ShaderStage stage) 
    : mModuleHandle(VK_NULL_HANDLE), mIsSource(src), mFile(file), mStage(stage), mShaderSource(nullptr), mShaderSourceSize(0) {
}

Shader::ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(Context::GetDeviceHandle(), mModuleHandle, nullptr);

    delete mShaderSource;
}

void Shader::ShaderModule::Reload(bool reCompile) {
    vkDestroyShaderModule(Context::GetDeviceHandle(), mModuleHandle, nullptr);
    delete mShaderSource;

    uint64_t size;
    char* data = (char*)Util::ReadFile(mFile, size);

    if (data == nullptr) {
        GM_LOG_CRITICAL("Failed to load shader file \"{0}\"", mFile.c_str());
        return;
    }

    VkShaderModuleCreateInfo mInfo;

    mInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    mInfo.pNext = nullptr;
    mInfo.flags = 0;

    if (mIsSource) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.SetGenerateDebugInfo();

        shaderc::CompilationResult result = compiler.CompileGlslToSpv(data, size, ShaderStageToShaderC(mStage), mFile.c_str(), options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            GM_LOG_CRITICAL(result.GetErrorMessage());
            GM_VERIFY(false);
        }

        std::vector<uint32_t> spirv(result.begin(), result.end());

        delete[] data;

        data = new char[spirv.size() * 4];
        memcpy(data, spirv.data(), spirv.size() * 4);

        mInfo.codeSize = spirv.size() * 4;
        mInfo.pCode = (uint32_t*)data;

    } else {
        if (size & 0x03) {
            GM_LOG_WARNING("Shader binary \"{0}\" size is not a multiple of 4", mFile.c_str());

            uint64_t newSize = (size & ~0x03) + 4;
            char* newData = new char[newSize];

            memcpy(newData, data, size);

            delete[] data;

            data = newData;
            size = newSize;
        }

        mInfo.codeSize = size;
        mInfo.pCode = (uint32_t*)data;
    }

    VK(vkCreateShaderModule(Context::GetDeviceHandle(), &mInfo, nullptr, &mModuleHandle));

    mShaderSourceSize = (uint32_t)mInfo.codeSize;
    mShaderSource = (uint32_t*)mInfo.pCode;
}

Shader::Shader() {
    
}

Shader::~Shader() {
    for (auto& [set, layout] : mDescriptorSetLayouts)
        delete layout;

    for (DescriptorPool* pool : mDescriptorPools)
        delete pool;
}

void Shader::Reload(bool reCompile) {
    for (ShaderModule& shader : mModules) {
        shader.Reload(reCompile);
    }

    for (auto& [set, layout] : mDescriptorSetLayouts)
        delete layout;

    for (DescriptorPool* pool : mDescriptorPools)
        delete pool;

    mDescriptorSetLayouts.clear();

    ReflectStages();
    CreateDescriptorSetLayouts();

}

void Shader::AddModule(const std::string& file, bool src, ShaderStage stage) {
    for (ShaderModule& shader : mModules) {
        GM_VERIFY(shader.mStage != stage);
    }

    mModules.emplace_back(file, src, stage);
}

void Shader::Compile() {
    for (ShaderModule& shader : mModules) {
        shader.Reload(false);
    }

    ReflectStages();
    CreateDescriptorSetLayouts();
}

VkShaderModule Shader::GetHandle(ShaderStage stage) const {
    for (const ShaderModule& shader : mModules) {
        if (shader.mStage == stage) return shader.mModuleHandle;
    }

    return VK_NULL_HANDLE;
}

std::vector<VkVertexInputAttributeDescription> Shader::GetVertexInputLayout(std::vector<std::pair<uint32_t, std::vector<uint32_t>>> locations) const {
    std::vector<VkVertexInputAttributeDescription> result;


    VkVertexInputAttributeDescription info;
    
    for (auto& binding : locations) {
        info.binding = binding.first;
        info.offset = 0;

        // Maka sure locations are in order to properly calculate offset
        std::sort(binding.second.begin(), binding.second.end()); 

        for (uint32_t location : binding.second) {
            info.location = location;
            info.format = VK_FORMAT_UNDEFINED;

            for (auto& input : mStageInputs) {
                if (input.Location == info.location) {
                    info.format = Util::SPIRTypeToVkFormat(input.Type);
                    result.push_back(info);

                    info.offset += input.Type.vecsize * (input.Type.width / 8);

                    break;
                }
            }

            GM_VERIFY(info.format != VK_FORMAT_UNDEFINED);

        }
    }

    return result;;
}

DescriptorSetLayout* Shader::GetDescriptorSetLayout(uint32_t set) const {
    GM_ASSERT(set < mDescriptorSetLayouts.size());

    for (auto& [setNum, layout] : mDescriptorSetLayouts) {
        if (setNum == set) return layout;
    }

    GM_LOG_CRITICAL("DescriptorSetLayout {0} doesn't exist", set);
    GM_ASSERT(false);

    return nullptr;
}

DescriptorSet** Shader::AllocateDescriptorSets(uint32_t set, uint32_t num) {
    DescriptorPool* pool = new DescriptorPool(num);

    mDescriptorPools.push_back(pool);

    return pool->AllocateDescriptorSets(GetDescriptorSetLayout(set), num);
}

void Shader::ReflectStages() {
    
    for (ShaderModule& shader : mModules) {
        spirv_cross::Compiler compiler(shader.mShaderSource, shader.mShaderSourceSize / 4);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();
        
        for (auto& input : resources.stage_inputs) {
            uint32_t location = compiler.get_decoration(input.id, spv::DecorationLocation);
            spirv_cross::SPIRType type = compiler.get_type(input.type_id);

            mStageInputs.emplace_back(location, type);
        }

        for (auto& uniform : resources.uniform_buffers) {
            uint32_t set = compiler.get_decoration(uniform.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(uniform.id, spv::DecorationBinding);
            spirv_cross::SPIRType type = compiler.get_type(uniform.type_id);

            GM_VERIFY(type.basetype == spirv_cross::SPIRType::Struct);

            uint32_t size = (uint32_t)compiler.get_declared_struct_size(type);

            std::vector<UniformBufferType::Member> members;

            uint32_t index = 0;
            uint32_t offset = 0;
            for (spirv_cross::TypeID id : type.member_types) {
                spirv_cross::SPIRType memberType = compiler.get_type(id);
                UniformBufferType::Member member;

                uint32_t memberSize = (uint32_t)compiler.get_declared_struct_member_size(type, index++);

                member.Name = compiler.get_name(id);
                member.Size = memberSize;
                member.Offset = offset;

                offset += member.Size;

                members.push_back(member);
            }

            mUniformBuffers.emplace_back(compiler.get_name(uniform.id), shader.mStage, set, binding, size, std::move(members));
        }

        for (auto& image : resources.sampled_images) {
            uint32_t set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(image.id, spv::DecorationBinding);
            spirv_cross::SPIRType type = compiler.get_type(image.type_id);
            uint32_t count = type.array.empty() ? 1 : type.array[0];


            mSampledImages.emplace_back(compiler.get_name(image.id), shader.mStage, set, binding, count, type.image);
        }
    }
}

void Shader::CreateDescriptorSetLayouts() {

    struct BindingIndex {
        BindingIndex(VkDescriptorSetLayoutBinding binding, UniformBaseType* uniform)
            : Binding(binding), Uniform(uniform) {}

        VkDescriptorSetLayoutBinding Binding;
        UniformBaseType* Uniform;
    };

    std::vector<std::pair<uint32_t, std::vector<BindingIndex>>> sets;
    VkDescriptorSetLayoutBinding binding;

    auto AddBindingToSet = [&sets, &binding](uint32_t set, UniformBaseType* uniform) {
        for (auto& [listSet, bindings] : sets) {
            if (set == listSet) {
                bindings.emplace_back(binding, uniform);
                break;
            }
        }

        std::vector<BindingIndex> tmp = { {binding , uniform} };

        sets.emplace_back(set, std::move(tmp));
    };

    binding.pImmutableSamplers = nullptr;
    binding.descriptorCount = 1;

    for (UniformBufferType& buf : mUniformBuffers) {
        binding.stageFlags = ShaderStageToVkShaderStage(buf.mStage);
        binding.binding = buf.mBinding;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        AddBindingToSet(buf.mSet, &buf);
    }

    for (SampledImageType& img : mSampledImages) {
        binding.stageFlags = ShaderStageToVkShaderStage(img.mStage);
        binding.binding = img.mBinding;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.descriptorCount = img.mArrayCount;

        AddBindingToSet(img.mSet, &img);
    }

    if (sets.empty()) {
        mDescriptorSetLayouts.emplace_back(0, new DescriptorSetLayout);
    }


    uint32_t index = 0;
    for (auto& [set, binding] : sets) {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        std::vector<UniformBaseType*> uniforms;

        for (BindingIndex& index : binding) {
            bindings.push_back(index.Binding);
            uniforms.push_back(index.Uniform);
        }

        DescriptorSetLayout* layout = new DescriptorSetLayout(bindings, uniforms);

        mDescriptorSetLayouts.emplace_back(set, layout);
    }
}
    
}