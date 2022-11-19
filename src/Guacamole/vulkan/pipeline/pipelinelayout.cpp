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

#include "pipelinelayout.h"

#include <Guacamole/vulkan/device.h>

namespace Guacamole {
PipelineLayout::PipelineLayout(Device* device, DescriptorSetLayout* layout, std::vector<VkPushConstantRange> pushConstants) : mDevice(device) {
    VkDescriptorSetLayout l = layout->GetHandle();

    VkPipelineLayoutCreateInfo lInfo;

    lInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    lInfo.pNext = nullptr;
    lInfo.flags = 0;
    lInfo.setLayoutCount = 1;
    lInfo.pSetLayouts = &l;
    lInfo.pushConstantRangeCount = (uint32_t)pushConstants.size();
    lInfo.pPushConstantRanges = pushConstants.data();

    VK(vkCreatePipelineLayout(mDevice->GetHandle(), &lInfo, nullptr, &mLayoutHandle));
}

PipelineLayout::PipelineLayout(Device* device, const std::vector<DescriptorSetLayout*>& layouts, std::vector<VkPushConstantRange> pushConstants) : mLayoutHandle(VK_NULL_HANDLE), mDevice(device) {
    VkPipelineLayoutCreateInfo lInfo;

    lInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    lInfo.pNext = nullptr;
    lInfo.flags = 0;
    lInfo.setLayoutCount = (uint32_t)layouts.size();

    std::vector<VkDescriptorSetLayout> tmp;

    for (DescriptorSetLayout* l : layouts) {
        tmp.push_back(l->GetHandle());
    }

    lInfo.pSetLayouts = tmp.data();
    lInfo.pushConstantRangeCount = (uint32_t)pushConstants.size();
    lInfo.pPushConstantRanges = pushConstants.data();nullptr;

    VK(vkCreatePipelineLayout(mDevice->GetHandle(), &lInfo, nullptr, &mLayoutHandle));
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(mDevice->GetHandle(), mLayoutHandle, nullptr);
}

}