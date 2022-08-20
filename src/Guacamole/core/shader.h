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

enum class ShaderStage {
    Vertex,
    Fragment,
    Geometry,
    Compute,
};

class Shader {
public:
    Shader(const std::string& file, bool src, ShaderStage stage);
    ~Shader();

    void Reload(bool reCompile = false);
    std::vector<VkVertexInputAttributeDescription> GetVertexInputLayout(std::vector<std::pair<uint32_t, std::vector<uint32_t>>> locations) const;
    DescriptorSetLayout* GetDescriptorSetLayout(uint32_t set) const;
    DescriptorSet** AllocateDescriptorSets(uint32_t set, uint32_t num);

    inline VkShaderModule GetHandle() const { return ModuleHandle; }

private:
    VkShaderModule ModuleHandle;

    ShaderStage Stage;
    bool IsSource;
    std::string File;

    struct StageInput {
        StageInput(uint8_t location, spirv_cross::SPIRType type) : Location(location), Type(type) {}

        uint8_t Location;
        spirv_cross::SPIRType Type;
    };

    struct UniformBuffer {
        UniformBuffer() {}
        UniformBuffer(uint32_t set, uint32_t binding, uint32_t size, std::vector<spirv_cross::SPIRType> members) : 
            Set(set), Binding(binding), Size(size), Members(members) {}

        uint32_t Set;
        uint32_t Binding;
        uint32_t Size;
        std::vector<spirv_cross::SPIRType> Members;
    };

    struct SampledImage {
        SampledImage(uint32_t set, uint32_t binding, uint32_t arrayCount, spirv_cross::SPIRType::ImageType image) : 
            Set(set), Binding(binding), ArrayCount(arrayCount), Image(image) {}

        uint32_t Set;
        uint32_t Binding;
        uint32_t ArrayCount;
        spirv_cross::SPIRType::ImageType Image;
    };

    std::vector<StageInput> StageInputs;
    std::vector<UniformBuffer> UniformBuffers;
    std::vector<SampledImage> SampledImages;

    std::vector<std::pair<uint32_t, DescriptorSetLayout*>> DescriptorSetLayouts;
    std::vector<DescriptorPool*> DescriptorPools;

private:
    void CreateDescriptorSetLayouts();

};

}