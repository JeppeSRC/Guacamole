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

#include "stagingbuffer.h"
#include "commandpoolmanager.h"

#include <Guacamole/vulkan/util.h>

namespace Guacamole {

StagingBuffer::StagingBuffer(uint64_t size, CommandBuffer* commandBuffer) 
    : mAllocated(0),
      mBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
      mCommandBuffer(commandBuffer) 
{

    mMemory = (uint8_t*)mBuffer.Map();

    if (mCommandBuffer == nullptr) {
        mCommandBuffer = CommandPoolManager::AllocateAuxCommandBuffer(std::this_thread::get_id(), true);
    }

}

StagingBuffer::~StagingBuffer() {
    mBuffer.Unmap();
}

void StagingBuffer::Begin(bool wait) {
    if (wait) mCommandBuffer->WaitForFence();
    mCommandBuffer->Begin(true);
}

void* StagingBuffer::Allocate(uint64_t size, Buffer* buffer, uint64_t bufferOffset) {
    GM_ASSERT_MSG(size + mAllocated <= GetSize(), "Buffer size exceeded");
    GM_ASSERT_MSG((bufferOffset + size) <= buffer->GetSize(), "Offset + size larager than buffer size");

    uint8_t* mem = mMemory + mAllocated;

    VkBufferCopy copy;

    copy.size = size;
    copy.dstOffset = bufferOffset;
    copy.srcOffset = mAllocated;

    vkCmdCopyBuffer(mCommandBuffer->GetHandle(), mBuffer.GetHandle(), buffer->GetHandle(), 1, &copy);

    mAllocated += size;

    return mem;
}

void* StagingBuffer::AllocateImage(VkImageLayout oldLayout, VkImageLayout newLayout, Texture* texture, uint32_t mip) {
    uint64_t size = texture->GetImageBufferSize();
    GM_ASSERT_MSG(size + mAllocated <= GetSize(), "Buffer size exceeded");

    uint64_t texelSize = GetFormatSize(texture->GetViewInfo().format);
    uint64_t mask = ~15ULL;
    
    // Align to 16 bytes to make sure it's always a multiple of texel size
    mAllocated = (mAllocated + texelSize) & mask;

    uint8_t* mem = mMemory + mAllocated;

    const VkImageViewCreateInfo& viewInfo = texture->GetViewInfo();

    VkBufferImageCopy copy;

    copy.bufferOffset = mAllocated;
    copy.bufferImageHeight = 0;
    copy.bufferRowLength = 0;
    copy.imageSubresource.aspectMask = viewInfo.subresourceRange.aspectMask;
    copy.imageSubresource.baseArrayLayer = viewInfo.subresourceRange.baseArrayLayer;
    copy.imageSubresource.layerCount = viewInfo.subresourceRange.layerCount;
    copy.imageSubresource.mipLevel = mip;
    copy.imageOffset = {};
    copy.imageExtent = texture->GetImageInfo().extent;

    texture->Transition(oldLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mCommandBuffer);
    vkCmdCopyBufferToImage(mCommandBuffer->GetHandle(), mBuffer.GetHandle(), texture->GetImageHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    texture->Transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, newLayout, mCommandBuffer);

    mAllocated += size;

    return mem;
}

void StagingBuffer::Reset() {
    mAllocated = 0;
}

std::unordered_map<std::thread::id, StagingBuffer*> StagingBuffer::mStagingBuffers;

void StagingBuffer::AllocateStagingBuffer(std::thread::id id, uint64_t size) {
    auto it = mStagingBuffers.find(id);

    GM_ASSERT_MSG(it == mStagingBuffers.end(), "This may onle be done once per thread");

    mStagingBuffers.emplace(id, new StagingBuffer(size));
}

void StagingBuffer::FreeBuffers() {
    for (auto [id, buf] : mStagingBuffers) {
        buf->GetCommandBuffer()->WaitForFence();
        delete buf;
    }

    mStagingBuffers.clear();
}

}