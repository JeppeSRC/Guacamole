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

        inline VkShaderModule GetHandle() const { return mModuleHandle; }
        inline ShaderStage GetStage() const { return mStage; }

    private:
        VkShaderModule mModuleHandle;

        ShaderStage mStage;
        bool mIsSource;
        std::string mFile;

        uint32_t mShaderSourceSize;
        uint32_t* mShaderSource;

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
    const std::vector<VkPushConstantRange>& GetPushConstants() const { return mPushConstants; }
    std::vector<DescriptorSetLayout*> GetDescriptorSetLayouts() const;
    std::vector<DescriptorSet> AllocateDescriptorSets(uint32_t set, uint32_t num);
    DescriptorSet AllocateDescriptorSet(uint32_t set);

private:
    std::vector<ShaderModule> mModules;

    struct StageInput {
        StageInput(uint8_t location, spirv_cross::SPIRType type) : Location(location), Type(type) {}

        uint8_t Location;
        spirv_cross::SPIRType Type;
    };

    std::vector<StageInput> mStageInputs;
    std::vector<UniformBufferType> mUniformBuffers;
    std::vector<SampledImageType> mSampledImages;
    std::vector<VkPushConstantRange> mPushConstants;

    std::vector<std::pair<uint32_t, DescriptorSetLayout*>> mDescriptorSetLayouts;
    std::vector<DescriptorPool*> mDescriptorPools;

private:
    void ReflectStages();
    void CreateDescriptorSetLayouts();
};

}