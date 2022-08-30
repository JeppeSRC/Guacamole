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

#include "shader_uniform.h"

namespace Guacamole {

class DescriptorSetLayout {
public:
    DescriptorSetLayout();
    DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, std::vector<UniformBaseType*> uniformLayout);
    ~DescriptorSetLayout();

    const UniformBufferType* GetUniformBuffer(uint32_t binding) const;
    const UniformBufferType::Member* GetUniformBufferMember(uint32_t binding, uint32_t memberIndex) const;
    uint32_t GetUniformBufferSize(uint32_t binding) const;
    uint32_t GetUniformBufferMemeberCount(uint32_t binding) const;
    uint32_t GetUniformBufferMemberSize(uint32_t binding, uint32_t memberIndex) const;
    uint32_t GetUniformBufferMemberOffset(uint32_t binding, uint32_t memberIndex) const;

    inline VkDescriptorSetLayout GetHandle() const { return mDescriptorSetLayoutHandle; }
    inline std::vector<VkDescriptorSetLayoutBinding> GetBindings() const { return mBindings; }
    inline VkDescriptorSetLayoutBinding GetBinding(uint32_t binding) const { return mBindings[binding]; }
private:
    VkDescriptorSetLayout mDescriptorSetLayoutHandle;
    std::vector<VkDescriptorSetLayoutBinding> mBindings;

    std::vector<UniformBaseType*> mUniformLayout;
};

class DescriptorSet {
private:
    friend class DescriptorSetLayout;
public:
    DescriptorSet(VkDescriptorSet Handle, DescriptorSetLayout* layout);
    ~DescriptorSet();

    inline VkDescriptorSet GetHandle() const { return mDescriptorSetHandle; }
    inline DescriptorSetLayout* GetLayout() const { return mLayout; }
private:
    VkDescriptorSet mDescriptorSetHandle;
    DescriptorSetLayout* mLayout;

};

class DescriptorPool {
public:
    DescriptorPool(uint32_t maxSets);
    ~DescriptorPool();

    DescriptorSet* AllocateDescriptorSet(DescriptorSetLayout* layout);
    DescriptorSet** AllocateDescriptorSets(DescriptorSetLayout* layout, uint32_t num);

    inline VkDescriptorPool GetHandle() const { return mPoolHandle; }

private:
    VkDescriptorPool mPoolHandle;
};

}