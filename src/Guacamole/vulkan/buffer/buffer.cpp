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
#include <Guacamole/vulkan/context.h>
#include <Guacamole/vulkan/swapchain.h>
#include "commandpoolmanager.h"

namespace Guacamole {

Buffer::Buffer(VkBufferUsageFlags usage, uint64_t size, VkMemoryPropertyFlags flags) : 
    mBufferSize(size), mMappedMemory(nullptr), mBufferFlags(flags) {

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
    aInfo.memoryTypeIndex = GetMemoryIndex(Context::GetPhysicalDevice()->GetMemoryProperties(), memReq.memoryTypeBits, flags);

    VK(vkAllocateMemory(Context::GetDeviceHandle(), &aInfo, nullptr, &mBufferMemory));
    VK(vkBindBufferMemory(Context::GetDeviceHandle(), mBufferHandle, mBufferMemory, 0));
}

Buffer::Buffer(Buffer&& other) : 
    mBufferHandle(other.mBufferHandle), mBufferMemory(other.mBufferMemory), mBufferFlags(other.mBufferFlags),
    mBufferSize(other.mBufferSize), mMappedMemory(other.mMappedMemory) 
{
    other.mBufferHandle = VK_NULL_HANDLE;
    other.mMappedMemory = VK_NULL_HANDLE;
}

Buffer::~Buffer() {
    vkFreeMemory(Context::GetDeviceHandle(), mBufferMemory, nullptr);
    vkDestroyBuffer(Context::GetDeviceHandle(), mBufferHandle, nullptr);
}

void* Buffer::Map() {
    GM_ASSERT_MSG(mBufferFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "Buffer must've been created with VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT to be mappable");
    if (mMappedMemory) return mMappedMemory;

    VK(vkMapMemory(Context::GetDeviceHandle(), mBufferMemory, 0, mBufferSize, 0, &mMappedMemory));

    return mMappedMemory;
}

void Buffer::Unmap() {
    GM_ASSERT_MSG(mBufferFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "Buffer must've been created with VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT to be mappable");
    vkUnmapMemory(Context::GetDeviceHandle(), mBufferMemory);
}

uint32_t Buffer::GetMemoryIndex(const VkPhysicalDeviceMemoryProperties props, uint32_t type, VkMemoryPropertyFlags flags) {
    for (uint32_t i = 0; i < props.memoryTypeCount; i++) {
        if ((type & (1 << i)) && (props.memoryTypes[i].propertyFlags & flags)) return i;
    }

    GM_ASSERT(false)

    return ~0;
}

}

