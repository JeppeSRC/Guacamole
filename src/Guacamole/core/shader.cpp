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

Shader::Shader(const std::string& file, bool src, ShaderStage stage) : ModuleHandle(VK_NULL_HANDLE), IsSource(src), File(file), Stage(stage) {
    Reload();
}

Shader::~Shader() {
    vkDestroyShaderModule(Context::GetDeviceHandle(), ModuleHandle, nullptr);

    for (auto& [set, layout] : DescriptorSetLayouts)
        delete layout;

    for (DescriptorPool* pool : DescriptorPools)
        delete pool;
}

void Shader::Reload(bool reCompile) {
    vkDestroyShaderModule(Context::GetDeviceHandle(), ModuleHandle, nullptr);

    uint64_t size;
    char* data = (char*)Util::ReadFile(File, size);

    if (data == nullptr) {
        GM_LOG_CRITICAL("Failed to load shader file \"{0}\"", File.c_str());
        return;
    }

    VkShaderModuleCreateInfo mInfo;

    mInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    mInfo.pNext = nullptr;
    mInfo.flags = 0;

    if (IsSource) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.SetGenerateDebugInfo();

        shaderc::CompilationResult result = compiler.CompileGlslToSpv(data, size, ShaderStageToShaderC(Stage), File.c_str(), options);

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
            GM_LOG_WARNING("Shader binary \"{0}\" size is not a multiple of 4", File.c_str());

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

    VK(vkCreateShaderModule(Context::GetDeviceHandle(), &mInfo, nullptr, &ModuleHandle));

    spirv_cross::Compiler compiler(mInfo.pCode, mInfo.codeSize / 4);

    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    GM_LOG_DEBUG("Shader reflection");

    GM_LOG_DEBUG("Stage Inputs: ");

    for (auto& input : resources.stage_inputs) {
        GM_LOG_DEBUG("\tName: {0} ", input.name.c_str());

        uint32_t location = compiler.get_decoration(input.id, spv::DecorationLocation);
        spirv_cross::SPIRType type = compiler.get_type(input.type_id);

        StageInputs.emplace_back(location, type);
    }

    GM_LOG_DEBUG("Stage Outputs: ");

    for (auto& output : resources.stage_outputs) {
        GM_LOG_DEBUG("\tName: {0} ", output.name.c_str());
    }

    GM_LOG_DEBUG("Uniform Buffers: ");

    for (auto& uniform : resources.uniform_buffers) {
        GM_LOG_DEBUG("\tName: {0} ", uniform.name.c_str());

        uint32_t set = compiler.get_decoration(uniform.id, spv::DecorationDescriptorSet);
        uint32_t binding = compiler.get_decoration(uniform.id, spv::DecorationBinding);
        spirv_cross::SPIRType type = compiler.get_type(uniform.type_id);

        GM_VERIFY(type.basetype == spirv_cross::SPIRType::Struct);

        UniformBuffer buf;

        buf.Set = set;
        buf.Binding = binding;

        for (spirv_cross::TypeID id : type.member_types) {
            spirv_cross::SPIRType memberType = compiler.get_type(id);

            buf.Members.push_back(memberType);
        }

        UniformBuffers.push_back(buf);
    }

    GM_LOG_DEBUG("Sampled Images: ");
    
    for (auto& image : resources.sampled_images) {
        GM_LOG_DEBUG("\tName: {0} ", image.name.c_str());

        uint32_t set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
        uint32_t binding = compiler.get_decoration(image.id, spv::DecorationBinding);
        spirv_cross::SPIRType type = compiler.get_type(image.type_id);
        uint32_t count = type.array.empty() ? 0 : type.array[0];


        SampledImages.emplace_back(set, binding, count, type.image);
    }

    delete[] data;

    CreateDescriptorSetLayouts();
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

            for (auto& input : StageInputs) {
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
    GM_ASSERT(set < DescriptorSetLayouts.size());

    for (auto& [setNum, layout] : DescriptorSetLayouts) {
        if (setNum == set) return layout;
    }

    GM_LOG_CRITICAL("DescriptorSetLayout {0} doesn't exist", set);
    GM_ASSERT(false);

    return nullptr;
}

DescriptorSet** Shader::AllocateDescriptorSets(uint32_t set, uint32_t num) {
    DescriptorPool* pool = new DescriptorPool(num);

    DescriptorPools.push_back(pool);

    return pool->AllocateDescriptorSets(GetDescriptorSetLayout(set), num);
}

void Shader::CreateDescriptorSetLayouts() {
    
    std::vector<std::pair<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>> Sets;
    VkDescriptorSetLayoutBinding binding;

    auto AddBindingToSet = [&Sets, &binding](uint32_t set) {
        for (auto& [listSet, bindings] : Sets) {
            if (set == listSet) {
                bindings.push_back(binding);
                break;
            }
        }

        std::vector<VkDescriptorSetLayoutBinding> tmp = { binding };

        Sets.emplace_back(set, std::move(tmp));
    };


    binding.stageFlags = ShaderStageToVkShaderStage(Stage);
    binding.pImmutableSamplers = nullptr;
    binding.descriptorCount = 1;

    for (UniformBuffer& buf : UniformBuffers) {
        binding.binding = buf.Binding;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        AddBindingToSet(buf.Set);
    }

    for (SampledImage& img : SampledImages) {
        binding.binding = img.Binding;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.descriptorCount = img.ArrayCount;

        AddBindingToSet(img.Set);
    }

    for (auto& [set, bindings] : Sets) {
        DescriptorSetLayout* layout = new DescriptorSetLayout(bindings);

        DescriptorSetLayouts.emplace_back(set, layout);
    }
}
    
}