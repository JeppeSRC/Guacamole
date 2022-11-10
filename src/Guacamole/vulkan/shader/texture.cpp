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

#include <Guacamole/vulkan/swapchain.h>
#include <Guacamole/vulkan/buffer/commandpoolmanager.h>
#include <Guacamole/vulkan/context.h>
#include <Guacamole/util/util.h>
#include <Guacamole/vulkan/util.h>
#include <Guacamole/vulkan/buffer/stagingbuffer.h>

#include <stb_image.h>


namespace Guacamole {



Texture::Texture(const std::filesystem::path& path) 
    : Asset(path, AssetType::Texture),
    mImageMemory(VK_NULL_HANDLE), mImageHandle(VK_NULL_HANDLE), mImageViewHandle(VK_NULL_HANDLE) {}

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
    aInfo.allocationSize = memReq.size;
    aInfo.memoryTypeIndex = Buffer::GetMemoryIndex(Context::GetPhysicalDevice()->GetMemoryProperties(), memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK(vkAllocateMemory(Context::GetDeviceHandle(), &aInfo, nullptr, &mImageMemory));
    VK(vkBindImageMemory(Context::GetDeviceHandle(), mImageHandle, mImageMemory, 0));
}

Texture::~Texture() {
    vkFreeMemory(Context::GetDeviceHandle(), mImageMemory, nullptr);
    vkDestroyImage(Context::GetDeviceHandle(), mImageHandle, nullptr);
    vkDestroyImageView(Context::GetDeviceHandle(), mImageViewHandle, nullptr);
}

void Texture::Transition(VkImageLayout oldLayout, VkImageLayout newLayout, CommandBuffer* commandBuffer) {

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

    VkPipelineStageFlags src;
    VkPipelineStageFlags dst;

    switch (oldLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            bar.srcAccessMask = 0;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            src = VK_PIPELINE_STAGE_TRANSFER_BIT;
            bar.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            src = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            bar.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        default:
            GM_ASSERT_MSG(false, "Transition not implemented");
            break;
    }

    switch (newLayout) {
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            bar.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
            bar.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        default:
            GM_ASSERT_MSG(false, "Transition not implemented");
            break;
    }

    vkCmdPipelineBarrier(commandBuffer->GetHandle(), src, dst, 0, 0, 0, 0, 0, 1, &bar);
    
}

uint64_t Texture::GetImageBufferSize() const {
    return GetWidth() * GetHeight() * GetFormatSize(mImageInfo.format);
}


void Texture2D::CreateImageView(VkFormat format) {
    mViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    mViewInfo.pNext = nullptr;
    mViewInfo.flags = 0;
    mViewInfo.image = mImageHandle;
    mViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    mViewInfo.format = format;
    mViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    mViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    mViewInfo.subresourceRange.baseMipLevel = 0;
    mViewInfo.subresourceRange.levelCount = 1;
    mViewInfo.subresourceRange.baseArrayLayer = 0;
    mViewInfo.subresourceRange.layerCount = 1;

    VK(vkCreateImageView(Context::GetDeviceHandle(), &mViewInfo, nullptr, &mImageViewHandle));
}

Texture2D::Texture2D(uint32_t width, uint32_t height, VkFormat format) : Texture("") {
    CreateImage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, { width, height, 1 }, VK_IMAGE_TYPE_2D, format, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_UNDEFINED);
    CreateImageView(format);

    mFlags |= AssetFlag_Loaded;
}

Texture2D::Texture2D(const std::filesystem::path& path) : Texture(path) {

}

void Texture2D::Load() {
    GM_ASSERT_MSG(!(mFlags & AssetFlag_Loaded), "Texture already loaded");

    LoadImageFromFile(mFilePath);
}

void Texture2D::Unload() {
    mFlags &= ~AssetFlag_Loaded;
}

void Texture2D::LoadImageFromMemory(uint8_t* data, uint64_t size) {
    GM_ASSERT(data);
    GM_ASSERT(size);

    LoadImageInternal(data, size);

    mFlags |= AssetFlag_Loaded;
}

void Texture2D::LoadImageFromFile(const std::filesystem::path& path) {
    GM_ASSERT(path.empty() == false);

    uint64_t fileSize = 0;

    uint8_t* data = Util::ReadFile(path, &fileSize);

    if (data == nullptr) return;

    LoadImageInternal(data, fileSize);

    mFlags |= AssetFlag_Loaded;

    delete data;
}

void Texture2D::LoadImageInternal(uint8_t* data, uint64_t size) {
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

    memcpy(StagingManager::GetCommonStagingBuffer()->AllocateImage(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this), pixels, width * height * 4);

    free(pixels);
}


}