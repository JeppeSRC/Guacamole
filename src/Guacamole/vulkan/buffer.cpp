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

#include "buffer.h"
#include "context.h"
#include "swapchain.h"
#include "commandpoolmanager.h"

namespace Guacamole {

Buffer::Buffer(VkBufferUsageFlags usage, uint64_t size, void* data) : 
    mMappedBufferHandle(VK_NULL_HANDLE), mMappedBufferMemory(VK_NULL_HANDLE), mBufferSize(size), mMappedMemory(nullptr) {

    VkBufferCreateInfo bInfo;

    bInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bInfo.pNext = nullptr;
    bInfo.flags = 0;
    bInfo.size = size;
    bInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bInfo.queueFamilyIndexCount = 0;
    bInfo.pQueueFamilyIndices = nullptr;

    VK(vkCreateBuffer(Context::GetDeviceHandle(), &bInfo, nullptr, &mBufferHandle));

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(Context::GetDeviceHandle(), mBufferHandle, &memReq);
    
    VkMemoryAllocateInfo aInfo;

    aInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    aInfo.pNext = nullptr;
    aInfo.allocationSize = memReq.size;
    aInfo.memoryTypeIndex = GetMemoryIndex(Context::GetPhysicalDevice()->GetMemoryProperties(), memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);;

    VK(vkAllocateMemory(Context::GetDeviceHandle(), &aInfo, nullptr, &mBufferMemory));
    VK(vkBindBufferMemory(Context::GetDeviceHandle(), mBufferHandle, mBufferMemory, 0));

    if (data) {
        WriteDataImmediate(data, size);
    }
}

Buffer::~Buffer() {
    vkFreeMemory(Context::GetDeviceHandle(), mBufferMemory, nullptr);
    vkFreeMemory(Context::GetDeviceHandle(), mMappedBufferMemory, nullptr);
    vkDestroyBuffer(Context::GetDeviceHandle(), mBufferHandle, nullptr);
    vkDestroyBuffer(Context::GetDeviceHandle(), mMappedBufferHandle, nullptr);
}

void* Buffer::Map() {
    if (mMappedMemory) return mMappedMemory;

    if (mMappedBufferMemory == VK_NULL_HANDLE) {
        VkBufferCreateInfo bInfo;

        bInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bInfo.pNext = nullptr;
        bInfo.flags = 0;
        bInfo.size = mBufferSize;
        bInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bInfo.queueFamilyIndexCount = 0;
        bInfo.pQueueFamilyIndices = nullptr;

        VK(vkCreateBuffer(Context::GetDeviceHandle(), &bInfo, nullptr, &mMappedBufferHandle));

        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(Context::GetDeviceHandle(), mMappedBufferHandle, &memReq);

        VkMemoryAllocateInfo aInfo;

        aInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        aInfo.pNext = nullptr;
        aInfo.allocationSize = memReq.size;
        aInfo.memoryTypeIndex = GetMemoryIndex(Context::GetPhysicalDevice()->GetMemoryProperties(), memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VK(vkAllocateMemory(Context::GetDeviceHandle(), &aInfo, nullptr, &mMappedBufferMemory));
        VK(vkBindBufferMemory(Context::GetDeviceHandle(), mMappedBufferHandle, mMappedBufferMemory, 0));
    }

    VK(vkMapMemory(Context::GetDeviceHandle(), mMappedBufferMemory, 0, mBufferSize, 0, &mMappedMemory));

    return mMappedMemory;
}

void Buffer::Unmap() {
    GM_ASSERT(mMappedMemory)

    vkUnmapMemory(Context::GetDeviceHandle(), mMappedBufferMemory);
    vkFreeMemory(Context::GetDeviceHandle(), mMappedBufferMemory, nullptr);
    vkDestroyBuffer(Context::GetDeviceHandle(), mMappedBufferHandle, nullptr);

    mMappedMemory = nullptr;
    mMappedBufferMemory = VK_NULL_HANDLE;
    mMappedBufferHandle = VK_NULL_HANDLE;
}

void Buffer::WriteData(void* data, uint64_t size, uint64_t offset) {
    void* mem = Map();

    memcpy((uint8_t*)mem + offset, data, size);

    StageCopy(false);
}

void Buffer::WriteDataImmediate(void* data, uint64_t size, uint64_t offset) {
    void* mem = Map();

    memcpy((uint8_t*)mem + offset, data, size);

    StageCopy(true);

    Unmap();
}

void Buffer::StageCopy(bool immediate) {
    if (immediate) {
        CommandBuffer* cmd = CommandPoolManager::GetCopyCommandBuffer(1);

        cmd->Begin(true);

        VkBufferCopy copy;

        copy.srcOffset = 0;
        copy.dstOffset = 0;
        copy.size = mBufferSize;

        vkCmdCopyBuffer(cmd->GetHandle(), mMappedBufferHandle, mBufferHandle, 1, &copy);

        cmd->End();

        VkSubmitInfo sInfo;

        VkCommandBuffer Handle = cmd->GetHandle();

        sInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        sInfo.pNext = nullptr;
        sInfo.waitSemaphoreCount = 0;
        sInfo.pWaitSemaphores = nullptr;
        sInfo.pWaitDstStageMask = nullptr;
        sInfo.commandBufferCount = 1;
        sInfo.pCommandBuffers = &Handle;
        sInfo.signalSemaphoreCount = 0;
        sInfo.pSignalSemaphores = nullptr;

        VK(vkQueueWaitIdle(Swapchain::GetGraphicsQueue()));
        VK(vkQueueSubmit(Swapchain::GetGraphicsQueue(), 1, &sInfo, VK_NULL_HANDLE));
        VK(vkQueueWaitIdle(Swapchain::GetGraphicsQueue()));

    } else {
        CommandBuffer* cmd = CommandPoolManager::GetCopyCommandBuffer();

        VkBufferCopy copy;

        copy.srcOffset = 0;
        copy.dstOffset = 0;
        copy.size = mBufferSize;

        vkCmdCopyBuffer(cmd->GetHandle(), mMappedBufferHandle, mBufferHandle, 1, &copy);
    }
}

uint32_t Buffer::GetMemoryIndex(const VkPhysicalDeviceMemoryProperties props, uint32_t type, VkMemoryPropertyFlags flags) {
    for (uint32_t i = 0; i < props.memoryTypeCount; i++) {
        if ((type & (1 << i)) && (props.memoryTypes[i].propertyFlags & flags)) return i;
    }

    GM_ASSERT(false)

    return ~0;
}

}

