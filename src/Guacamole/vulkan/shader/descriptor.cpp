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

#include "descriptor.h"

#include <Guacamole/vulkan/device.h>

namespace Guacamole {

DescriptorSet::DescriptorSet(VkDescriptorSet Handle, DescriptorSetLayout* layout) : mDescriptorSetHandle(Handle), mLayout(layout) {

}

DescriptorSet::~DescriptorSet() {

}

DescriptorSetLayout::DescriptorSetLayout(Device* device) : mDescriptorSetLayoutHandle(VK_NULL_HANDLE), mDevice(device) {
    VkDescriptorSetLayoutCreateInfo lInfo;

    lInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    lInfo.pNext = nullptr;
    lInfo.flags = 0;
    lInfo.bindingCount = 0;
    lInfo.pBindings = nullptr;

    VK(vkCreateDescriptorSetLayout(mDevice->GetHandle(), &lInfo, nullptr, &mDescriptorSetLayoutHandle));
}

DescriptorSetLayout::DescriptorSetLayout(Device* device, const std::vector<VkDescriptorSetLayoutBinding>& bindings, std::vector<UniformBaseType*> uniformLayout)
    : mDescriptorSetLayoutHandle(VK_NULL_HANDLE), mUniformLayout(uniformLayout), mDevice(device) {
    VkDescriptorSetLayoutCreateInfo lInfo;

    lInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    lInfo.pNext = nullptr;
    lInfo.flags = 0;
    lInfo.bindingCount = (uint32_t)bindings.size();
    lInfo.pBindings = bindings.data();

    VK(vkCreateDescriptorSetLayout(mDevice->GetHandle(), &lInfo, nullptr, &mDescriptorSetLayoutHandle));
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(mDevice->GetHandle(), mDescriptorSetLayoutHandle, nullptr);
}

const UniformBufferType* DescriptorSetLayout::GetUniformBuffer(uint32_t binding) const {
    for (UniformBaseType* base : mUniformLayout) {
        if (base->mBinding == binding) {
            GM_ASSERT(base->mType == UniformType::Buffer);

            return (UniformBufferType*)base;
        }
    }

    GM_VERIFY(false);

    return nullptr;
}

const UniformBufferType::Member* DescriptorSetLayout::GetUniformBufferMember(uint32_t binding, uint32_t memberIndex) const {
    const UniformBufferType* buf = GetUniformBuffer(binding);

    GM_ASSERT(memberIndex < buf->mMembers.size()) ;

    return &buf->mMembers[memberIndex];
}

uint32_t DescriptorSetLayout::GetUniformBufferSize(uint32_t binding) const {
    const UniformBufferType* buf = GetUniformBuffer(binding);

    return buf->mSize;
}

uint32_t DescriptorSetLayout::GetUniformBufferMemeberCount(uint32_t binding) const {
    const UniformBufferType* buf = GetUniformBuffer(binding);

    return (uint32_t)buf->mMembers.size();
}

uint32_t DescriptorSetLayout::GetUniformBufferMemberSize(uint32_t binding, uint32_t memberIndex) const {
    const UniformBufferType::Member* member = GetUniformBufferMember(binding, memberIndex);

    return member->Size;
}

uint32_t DescriptorSetLayout::GetUniformBufferMemberOffset(uint32_t binding, uint32_t memberIndex) const {
    const UniformBufferType::Member* member = GetUniformBufferMember(binding, memberIndex);

    return member->Offset;
}


DescriptorPool::DescriptorPool(Device* device, uint32_t maxSets) : mDevice(device) {
    VkDescriptorPoolSize poolSizes[10];

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1000;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1000;

    VkDescriptorPoolCreateInfo pInfo;

    pInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pInfo.pNext = nullptr;
    pInfo.flags = 0;
    pInfo.maxSets = maxSets;
    pInfo.poolSizeCount = 2;
    pInfo.pPoolSizes = poolSizes;

    VK(vkCreateDescriptorPool(mDevice->GetHandle(), &pInfo, nullptr, &mPoolHandle));
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) : mDevice(other.mDevice), mPoolHandle(other.mPoolHandle) {
    other.mPoolHandle = 0;
    other.mDevice = nullptr;
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(mDevice->GetHandle(), mPoolHandle, nullptr);
}

DescriptorSet DescriptorPool::AllocateDescriptorSet(DescriptorSetLayout* layout) {
    std::vector<DescriptorSet> sets = AllocateDescriptorSets(layout, 1);

    return std::move(sets[0]);
}

std::vector<DescriptorSet> DescriptorPool::AllocateDescriptorSets(DescriptorSetLayout* layout, uint32_t num) {
    VkDescriptorSetLayout* layouts = new VkDescriptorSetLayout[num];
    VkDescriptorSetLayout layoutHandle = layout->GetHandle();

    for (uint32_t i = 0; i < num; i++) {
        layouts[i] = layoutHandle;
    }

    VkDescriptorSetAllocateInfo aInfo;

    aInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    aInfo.pNext = nullptr;
    aInfo.descriptorPool = mPoolHandle;
    aInfo.descriptorSetCount = num;
    aInfo.pSetLayouts = layouts;

    VkDescriptorSet* set = new VkDescriptorSet[num];
    
    std::vector<DescriptorSet> sets;

    VK(vkAllocateDescriptorSets(mDevice->GetHandle(), &aInfo, set));

    for (uint32_t i = 0; i < num; i++) {
        sets.emplace_back(set[i], layout);
    }

    delete[] set;
    delete[] layouts;

    return sets;
}

void DescriptorPool::Reset() {
    vkResetDescriptorPool(mDevice->GetHandle(), mPoolHandle, 0);
}

}