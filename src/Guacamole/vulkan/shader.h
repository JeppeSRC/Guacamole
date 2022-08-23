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

#pragma once

#include <Guacamole.h>

#include "descriptor.h"

namespace Guacamole {


class Shader {
public:
    class ShaderModule {
    public:
        ShaderModule(const std::string& file, bool src, ShaderStage stage);
        ~ShaderModule();

        void Reload(bool reCompile = false);

        inline VkShaderModule GetHandle() const { return ModuleHandle; }
        inline ShaderStage GetStage() const { return Stage; }

    private:
        VkShaderModule ModuleHandle;

        ShaderStage Stage;
        bool IsSource;
        std::string File;

        uint32_t ShaderSourceSize;
        uint32_t* ShaderSource;

        friend class Shader;
    };

public:
    Shader();
    ~Shader();

    void Reload(bool reCompile = false);
    void AddModule(const std::string& file, bool src, ShaderStage stage);
    void Compile();

    VkShaderModule GetHandle(ShaderStage stage) const;

    std::vector<VkVertexInputAttributeDescription> GetVertexInputLayout(std::vector<std::pair<uint32_t, std::vector<uint32_t>>> locations) const;
    DescriptorSetLayout* GetDescriptorSetLayout(uint32_t set) const;
    DescriptorSet** AllocateDescriptorSets(uint32_t set, uint32_t num);

private:
    std::vector<ShaderModule> Modules;

    struct StageInput {
        StageInput(uint8_t location, spirv_cross::SPIRType type) : Location(location), Type(type) {}

        uint8_t Location;
        spirv_cross::SPIRType Type;
    };

    std::vector<StageInput> StageInputs;
    std::vector<UniformBufferType> UniformBuffers;
    std::vector<SampledImageType> SampledImages;

    std::vector<std::pair<uint32_t, DescriptorSetLayout*>> DescriptorSetLayouts;
    std::vector<DescriptorPool*> DescriptorPools;

private:
    void ReflectStages();
    void CreateDescriptorSetLayouts();
};

}