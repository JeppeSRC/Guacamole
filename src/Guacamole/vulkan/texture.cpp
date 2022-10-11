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

#include <stb_image.h>

#include "texture.h"
#include "context.h"
#include "swapchain.h"
#include "commandpoolmanager.h"
#include <Guacamole/util/util.h>


namespace Guacamole {



Texture::Texture(const std::filesystem::path& path) 
    : Asset(path, AssetType::Texture),
    mImageMemory(VK_NULL_HANDLE), mImageHandle(VK_NULL_HANDLE), mImageViewHandle(VK_NULL_HANDLE), 
      mMappedBufferHandle(VK_NULL_HANDLE), mMappedBufferMemory(VK_NULL_HANDLE), mMappedMemory(nullptr) {}

void Texture::CreateImage(VkImageUsageFlags usage, VkExtent3D extent, VkImageType imageType, VkFormat format, VkSampleCountFlagBits samples, VkImageLayout initialLayout) {
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageFormatProperties prop;

    bool supported = Context::GetPhysicalDevice()->CheckImageFormat(format, imageType, tiling, usage, &prop);

    if (!supported) {
        tiling = VK_IMAGE_TILING_LINEAR;
        
        supported = Context::GetPhysicalDevice()->CheckImageFormat(format, imageType, tiling, usage, &prop);

        if (supported) {
            GM_LOG_WARNING("Image format supported with linear tiling");
        } else {
            GM_LOG_CRITICAL("Image format not supported");
            GM_VERIFY(supported);
        }
    }
    
    mImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    mImageInfo.pNext = nullptr;
    mImageInfo.flags = 0;
    mImageInfo.imageType = imageType;
    mImageInfo.format = format;
    mImageInfo.extent = extent;
    mImageInfo.mipLevels = 1;
    mImageInfo.arrayLayers = 1;
    mImageInfo.samples = samples;
    mImageInfo.tiling = tiling;
    mImageInfo.usage = usage;
    mImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    mImageInfo.queueFamilyIndexCount = 0;
    mImageInfo.pQueueFamilyIndices = nullptr;
    mImageInfo.initialLayout = initialLayout;

    VK(vkCreateImage(Context::GetDeviceHandle(), &mImageInfo, nullptr, &mImageHandle));

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(Context::GetDeviceHandle(), mImageHandle, &memReq);

    VkMemoryAllocateInfo aInfo;

    aInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    aInfo.pNext = nullptr;
    aInfo.allocationSize = mImageMemorySize = memReq.size;
    aInfo.memoryTypeIndex = Buffer::GetMemoryIndex(Context::GetPhysicalDevice()->GetMemoryProperties(), memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK(vkAllocateMemory(Context::GetDeviceHandle(), &aInfo, nullptr, &mImageMemory));
    VK(vkBindImageMemory(Context::GetDeviceHandle(), mImageHandle, mImageMemory, 0));
}

Texture::~Texture() {
    vkFreeMemory(Context::GetDeviceHandle(), mImageMemory, nullptr);
    vkFreeMemory(Context::GetDeviceHandle(), mMappedBufferMemory, nullptr);
    vkDestroyImage(Context::GetDeviceHandle(), mImageHandle, nullptr);
    vkDestroyBuffer(Context::GetDeviceHandle(), mMappedBufferHandle, nullptr);
    vkDestroyImageView(Context::GetDeviceHandle(), mImageViewHandle, nullptr);
}

void* Texture::Map() {
    if (mMappedMemory) return mMappedMemory;

    if (mMappedBufferMemory == VK_NULL_HANDLE) {
        VkBufferCreateInfo bInfo;

        bInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bInfo.pNext = nullptr;
        bInfo.flags = 0;
        bInfo.size = mImageMemorySize;
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
        aInfo.memoryTypeIndex = Buffer::GetMemoryIndex(Context::GetPhysicalDevice()->GetMemoryProperties(), memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VK(vkAllocateMemory(Context::GetDeviceHandle(), &aInfo, nullptr, &mMappedBufferMemory));
        VK(vkBindBufferMemory(Context::GetDeviceHandle(), mMappedBufferHandle, mMappedBufferMemory, 0));
    }

    VK(vkMapMemory(Context::GetDeviceHandle(), mMappedBufferMemory, 0, mImageMemorySize, 0, &mMappedMemory));

    return mMappedMemory;
}

void Texture::Unmap() {
    GM_ASSERT(mMappedMemory);

    vkUnmapMemory(Context::GetDeviceHandle(), mMappedBufferMemory);
    vkFreeMemory(Context::GetDeviceHandle(), mMappedBufferMemory, nullptr);
    vkDestroyBuffer(Context::GetDeviceHandle(), mMappedBufferHandle, nullptr);

    mMappedBufferMemory = VK_NULL_HANDLE;
    mMappedBufferHandle = VK_NULL_HANDLE;

    mMappedMemory = nullptr;
}

void Texture::WriteData(void* data, uint64_t size, uint64_t offset) {
    void* mem = Map();

    memcpy((uint8_t*)mem + offset, data, size);

    StageCopy(false);
}

void Texture::WriteDataImmediate(void* data, uint64_t size, uint64_t offset) {
    void* mem = Map();

    memcpy((uint8_t*)mem + offset, data, size);

    StageCopy(true);

    Unmap();
}

void Texture::StageCopy(bool immediate) {
    if (immediate) {
        CommandBuffer* cmd = CommandPoolManager::GetCopyCommandBuffer(1);

        cmd->Begin(true);

        VkBufferImageCopy copy;

        copy.bufferOffset = 0;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;
        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.layerCount = 1;
        copy.imageSubresource.mipLevel = 0;
        copy.imageOffset = {};
        copy.imageExtent = mImageInfo.extent;

        vkCmdCopyBufferToImage(cmd->GetHandle(), mMappedBufferHandle, mImageHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

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

        VkBufferImageCopy copy;

        copy.bufferOffset = 0;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;
        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.layerCount = 1;
        copy.imageSubresource.mipLevel = 0;
        copy.imageOffset = {};
        copy.imageExtent = mImageInfo.extent;

        vkCmdCopyBufferToImage(cmd->GetHandle(), mMappedBufferHandle, mImageHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

    }

}

void Texture::Transition(VkImageLayout oldLayout, VkImageLayout newLayout, bool immediate) {
    if (immediate) {
        CommandBuffer* cmd = CommandPoolManager::GetCopyCommandBuffer(1);

        cmd->Begin(true);

        VkImageMemoryBarrier bar;

        bar.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        bar.pNext = nullptr;
        bar.srcAccessMask = 0;
        bar.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        bar.oldLayout = oldLayout;
        bar.newLayout = newLayout;
        bar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bar.image = mImageHandle;
        bar.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bar.subresourceRange.baseMipLevel = 0;
        bar.subresourceRange.levelCount = 1;
        bar.subresourceRange.baseArrayLayer = 0;
        bar.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(cmd->GetHandle(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &bar);

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
        CommandBuffer* cmd = CommandPoolManager::GetCopyCommandBuffer(0);

        VkImageMemoryBarrier bar;

        bar.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        bar.pNext = nullptr;
        //bar.srcAccessMask = 0;
        //bar.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        bar.oldLayout = oldLayout;
        bar.newLayout = newLayout;
        bar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bar.image = mImageHandle;
        bar.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bar.subresourceRange.baseMipLevel = 0;
        bar.subresourceRange.levelCount = 1;
        bar.subresourceRange.baseArrayLayer = 0;
        bar.subresourceRange.layerCount = 1;

        VkPipelineStageFlags src, dst;

        if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
            bar.srcAccessMask = 0;
            bar.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            bar.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            bar.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            src = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }

        vkCmdPipelineBarrier(cmd->GetHandle(), src, dst, 0, 0, 0, 0, 0, 1, &bar);

    }
}




void Texture2D::CreateImageView(VkFormat format) {
    VkImageViewCreateInfo imageViewInfo;

    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.pNext = nullptr;
    imageViewInfo.flags = 0;
    imageViewInfo.image = mImageHandle;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = format;
    imageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    VK(vkCreateImageView(Context::GetDeviceHandle(), &imageViewInfo, nullptr, &mImageViewHandle));
}

Texture2D::Texture2D(uint32_t width, uint32_t height, VkFormat format) : Texture("") {
    CreateImage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, { width, height, 1 }, VK_IMAGE_TYPE_2D, format, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_UNDEFINED);
    CreateImageView(format);

    mLoaded = true;
}

Texture2D::Texture2D(const std::filesystem::path& path) : Texture(path) {

}

void Texture2D::StageCopy(bool immediate) {
    Transition(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, immediate);
    Texture::StageCopy(immediate);
    Transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, immediate);
}

void Texture2D::Load(bool immediate) {
    GM_ASSERT_MSG(mLoaded == false, "Texture already loaded");

    LoadImageFromFile(mFilePath, immediate);

    mLoaded = true;
}

void Texture2D::Unload() {
    mLoaded = false;
}

void Texture2D::LoadImageFromMemory(uint8_t* data, uint64_t size, bool immediate) {
    GM_ASSERT(data);
    GM_ASSERT(size);

    int32_t width;
    int32_t height;
    int32_t channels;

    uint8_t* pixels = stbi_load_from_memory(data, size, &width, &height, &channels, 4);

    if (pixels == nullptr) {
        GM_LOG_CRITICAL("stbi_load_from_memory({0}, {1}, {2}, {3}, {4}, {5}) failed", uint64_t(data), size, width, height, channels, 4);
        free(pixels);
    }

    CreateImage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, { (uint32_t)width, (uint32_t)height, 1 }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_UNDEFINED);
    CreateImageView(VK_FORMAT_R8G8B8A8_UNORM);

    if (immediate) {
        WriteDataImmediate(pixels, width * height * 4);
    } else {
        WriteData(pixels, width * height * 4);
    }

    free(pixels);
}

void Texture2D::LoadImageFromFile(const std::filesystem::path& path, bool immediate) {
    GM_ASSERT(path.empty() == false);

    uint64_t fileSize = 0;

    uint8_t* data = Util::ReadFile(path, &fileSize);

    if (data == nullptr) return;

    LoadImageFromMemory(data, fileSize, immediate);
    delete data;
}

}