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

#include "commandbuffer.h"

#include <Guacamole/vulkan/context.h>

namespace Guacamole {

CommandBuffer::CommandBuffer(VkCommandBuffer Handle) : mCommandBufferHandle(Handle), mUsed(false) {
    VkFenceCreateInfo fInfo;

    fInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fInfo.pNext = nullptr;
    fInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK(vkCreateFence(Context::GetDeviceHandle(), &fInfo, nullptr, &mFenceHandle));
}

CommandBuffer::~CommandBuffer() {
    vkDestroyFence(Context::GetDeviceHandle(), mFenceHandle, nullptr);
}


void CommandBuffer::Reset() const {
    VK(vkResetCommandBuffer(mCommandBufferHandle, 0));
}

void CommandBuffer::Begin(bool oneTimeSubmit) const {
    GM_ASSERT(mUsed == false);

    VkCommandBufferBeginInfo bInfo;

    bInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bInfo.pNext = nullptr;
    bInfo.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;
    bInfo.pInheritanceInfo = nullptr;
    
    VK(vkBeginCommandBuffer(mCommandBufferHandle, &bInfo));

    mUsed = true;
}

void CommandBuffer::End() const {
    GM_ASSERT(mUsed == true);

    VK(vkEndCommandBuffer(mCommandBufferHandle));

    mUsed = false;
}

void CommandBuffer::WaitForFence() const {
    VK(vkWaitForFences(Context::GetDeviceHandle(), 1, &mFenceHandle, false, ~0));
}

CommandPool::CommandPool() {
    VkCommandPoolCreateInfo info;

    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.queueFamilyIndex = Context::GetPhysicalDevice()->GetQueueIndex(VK_QUEUE_GRAPHICS_BIT);

    VK(vkCreateCommandPool(Context::GetDeviceHandle(), &info, nullptr, &mCommandPoolHandle));
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(Context::GetDeviceHandle(), mCommandPoolHandle, nullptr);
}

void CommandPool::Reset() const {
    VK(vkResetCommandPool(Context::GetDeviceHandle(), mCommandPoolHandle, 0));
}

std::vector<CommandBuffer*> CommandPool::AllocateCommandBuffers(uint32_t num, bool primary) const {
    VkCommandBufferAllocateInfo info;

    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    info.commandBufferCount = num;
    info.commandPool = mCommandPoolHandle;

    VkCommandBuffer* buffers = new VkCommandBuffer[num];

    VK(vkAllocateCommandBuffers(Context::GetDeviceHandle(), &info, buffers));

    std::vector<CommandBuffer*> result;

    for (uint32_t i = 0; i < num; i++) {
        result.push_back(new CommandBuffer(buffers[i]));
    }

    delete[] buffers;

    return result;
}


}