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

#undef min

#include <spirv_cross/spirv_glsl.hpp>
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

Shader::Shader(const std::string& file, bool src, ShaderStage stage) : ModuleHandle(VK_NULL_HANDLE), IsSource(src), File(file), Stage(stage) {
    Reload();
}

Shader::~Shader() {
    vkDestroyShaderModule(Context::GetDeviceHandle(), ModuleHandle, nullptr);
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



    delete[] data;
}
    
    }