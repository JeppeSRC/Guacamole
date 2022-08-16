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
#include "context.h"

namespace Guacamole {

DescriptorSet::DescriptorSet(VkDescriptorSet Handle, DescriptorSetLayout* layout) : DescriptorSetHandle(Handle), Layout(layout) {

}

DescriptorSet::~DescriptorSet() {

}

DescriptorSetLayout::DescriptorSetLayout() : DescriptorSetLayoutHandle(VK_NULL_HANDLE) {
    VkDescriptorSetLayoutCreateInfo lInfo;

    lInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    lInfo.pNext = nullptr;
    lInfo.flags = 0;
    lInfo.bindingCount = 0;
    lInfo.pBindings = nullptr;

    VK(vkCreateDescriptorSetLayout(Context::GetDeviceHandle(), &lInfo, nullptr, &DescriptorSetLayoutHandle));
}

DescriptorSetLayout::DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings) : DescriptorSetLayoutHandle(VK_NULL_HANDLE) {
    VkDescriptorSetLayoutCreateInfo lInfo;

    lInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    lInfo.pNext = nullptr;
    lInfo.flags = 0;
    lInfo.bindingCount = (uint32_t)bindings.size();
    lInfo.pBindings = bindings.data();

    VK(vkCreateDescriptorSetLayout(Context::GetDeviceHandle(), &lInfo, nullptr, &DescriptorSetLayoutHandle));
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(Context::GetDeviceHandle(), DescriptorSetLayoutHandle, nullptr);
}


DescriptorPool::DescriptorPool(uint32_t maxSets) : RemainingSets(maxSets) {
    VkDescriptorPoolCreateInfo pInfo;

    pInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pInfo.pNext = nullptr;
    pInfo.flags = 0;
    pInfo.maxSets = maxSets;
    pInfo.poolSizeCount = 0;
    pInfo.pPoolSizes = nullptr;

    VK(vkCreateDescriptorPool(Context::GetDeviceHandle(), &pInfo, nullptr, &PoolHandle));
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(Context::GetDeviceHandle(), PoolHandle, nullptr);
}

DescriptorSet* DescriptorPool::AllocateDescriptorSet(DescriptorSetLayout* layout) {
    GM_ASSERT(RemainingSets > 0);

    VkDescriptorSetLayout tmp = layout->GetHandle();

    VkDescriptorSetAllocateInfo aInfo;

    aInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    aInfo.pNext = nullptr;
    aInfo.descriptorPool = PoolHandle;
    aInfo.descriptorSetCount = 1;
    aInfo.pSetLayouts = &tmp;

    VkDescriptorSet set;

    VK(vkAllocateDescriptorSets(Context::GetDeviceHandle(), &aInfo, &set));

    return new DescriptorSet(set, layout);
}


}