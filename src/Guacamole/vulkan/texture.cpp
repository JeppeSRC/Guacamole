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
#include "texture.h"
#include "context.h"
#include "swapchain.h"


namespace Guacamole {



Texture::Texture() 
    : ImageMemory(VK_NULL_HANDLE), ImageHandle(VK_NULL_HANDLE), ImageViewHandle(VK_NULL_HANDLE), 
      MappedBufferHandle(VK_NULL_HANDLE), MappedBufferMemory(VK_NULL_HANDLE) {}

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
    
    ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageInfo.pNext = nullptr;
    ImageInfo.flags = 0;
    ImageInfo.imageType = imageType;
    ImageInfo.format = format;
    ImageInfo.extent = extent;
    ImageInfo.mipLevels = 1;
    ImageInfo.arrayLayers = 1;
    ImageInfo.samples = samples;
    ImageInfo.tiling = tiling;
    ImageInfo.usage = usage;
    ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageInfo.queueFamilyIndexCount = 0;
    ImageInfo.pQueueFamilyIndices = nullptr;
    ImageInfo.initialLayout = initialLayout;

    VK(vkCreateImage(Context::GetDeviceHandle(), &ImageInfo, nullptr, &ImageHandle));

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(Context::GetDeviceHandle(), ImageHandle, &memReq);

    VkMemoryAllocateInfo aInfo;

    aInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    aInfo.pNext = nullptr;
    aInfo.allocationSize = ImageMemorySize = memReq.size;
    aInfo.memoryTypeIndex = Buffer::GetMemoryIndex(Context::GetPhysicalDevice()->GetMemoryProperties(), memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK(vkAllocateMemory(Context::GetDeviceHandle(), &aInfo, nullptr, &ImageMemory));
    VK(vkBindImageMemory(Context::GetDeviceHandle(), ImageHandle, ImageMemory, 0));
}

Texture::~Texture() {
    vkFreeMemory(Context::GetDeviceHandle(), ImageMemory, nullptr);
    vkDestroyImage(Context::GetDeviceHandle(), ImageHandle, nullptr);
    vkDestroyImageView(Context::GetDeviceHandle(), ImageViewHandle, nullptr);
}

void* Texture::Map() {
    if (MappedMemory) return MappedMemory;

    if (MappedBufferMemory == VK_NULL_HANDLE) {
        VkBufferCreateInfo bInfo;

        bInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bInfo.pNext = nullptr;
        bInfo.flags = 0;
        bInfo.size = ImageMemorySize;
        bInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bInfo.queueFamilyIndexCount = 0;
        bInfo.pQueueFamilyIndices = nullptr;

        VK(vkCreateBuffer(Context::GetDeviceHandle(), &bInfo, nullptr, &MappedBufferHandle));

        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(Context::GetDeviceHandle(), MappedBufferHandle, &memReq);

        VkMemoryAllocateInfo aInfo;

        aInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        aInfo.pNext = nullptr;
        aInfo.allocationSize = memReq.size;
        aInfo.memoryTypeIndex = Buffer::GetMemoryIndex(Context::GetPhysicalDevice()->GetMemoryProperties(), memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VK(vkAllocateMemory(Context::GetDeviceHandle(), &aInfo, nullptr, &MappedBufferMemory));
        VK(vkBindBufferMemory(Context::GetDeviceHandle(), MappedBufferHandle, MappedBufferMemory, 0));
    }

    VK(vkMapMemory(Context::GetDeviceHandle(), MappedBufferMemory, 0, ImageMemorySize, 0, &MappedMemory));

    return MappedMemory;
}

void Texture::Unmap() {
    vkUnmapMemory(Context::GetDeviceHandle(), MappedBufferMemory);
    vkFreeMemory(Context::GetDeviceHandle(), MappedBufferMemory, nullptr);
    vkDestroyBuffer(Context::GetDeviceHandle(), MappedBufferHandle, nullptr);

    MappedBufferMemory = VK_NULL_HANDLE;
    MappedBufferHandle = VK_NULL_HANDLE;

    MappedMemory = nullptr;
}

void Texture::WriteData(void* data, uint64_t size, uint64_t offset) {
    void* mem = Map();

    memcpy((uint8_t*)mem + offset, data, size);

    StageCopy(true);

    Unmap();
}

void Texture::StageCopy(bool immediate) {
    if (immediate) {
        CommandBuffer* cmd = Context::GetAuxCmdBuffer();

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
        copy.imageExtent = ImageInfo.extent;

        vkCmdCopyBufferToImage(cmd->GetHandle(), MappedBufferHandle, ImageHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

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
        // TOOD
    }

}


Texture2D::Texture2D(uint32_t width, uint32_t height, VkFormat format) {
    CreateImage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, { width, height, 1 }, VK_IMAGE_TYPE_2D, format, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewInfo.pNext = nullptr;
    ImageViewInfo.flags = 0;
    ImageViewInfo.image = ImageHandle;
    ImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewInfo.format = format;
    ImageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    ImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ImageViewInfo.subresourceRange.baseMipLevel = 0;
    ImageViewInfo.subresourceRange.levelCount = 1;
    ImageViewInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewInfo.subresourceRange.layerCount = 1;

    VK(vkCreateImageView(Context::GetDeviceHandle(), &ImageViewInfo, nullptr, &ImageViewHandle));


}


}