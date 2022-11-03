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

#pragma once

#include <Guacamole.h>

#include "sampler.h"

#include <Guacamole/vulkan/buffer/buffer.h>
#include <Guacamole/vulkan/buffer/commandbuffer.h>
#include <Guacamole/asset/asset.h>

namespace Guacamole {

class Texture : public Asset {
protected:
    Texture(const std::filesystem::path& path);

    void CreateImage(VkImageUsageFlags usage, VkExtent3D extent, VkImageType imageType, VkFormat format, VkSampleCountFlagBits samples, VkImageLayout initialLayout);

public:
    virtual ~Texture();

    void Transition(VkImageLayout oldLayout, VkImageLayout newLayout, CommandBuffer* commandBuffer);
    uint64_t GetImageBufferSize() const;

    inline uint32_t GetWidth() const { return mImageInfo.extent.width; }
    inline uint32_t GetHeight() const { return mImageInfo.extent.height; }

    inline VkImage GetImageHandle() const { return mImageHandle; }
    inline VkImageView GetImageViewHandle() const { return mImageViewHandle; }

    inline const VkImageCreateInfo& GetImageInfo() const { return mImageInfo; }
    inline const VkImageViewCreateInfo& GetViewInfo() const { return mViewInfo; }
protected:
    VkImage mImageHandle;
    VkDeviceMemory mImageMemory;
    VkImageView mImageViewHandle;

    VkImageCreateInfo mImageInfo;
    VkImageViewCreateInfo mViewInfo;
};

class Texture2D : public Texture {
public:
    Texture2D(uint32_t width, uint32_t height, VkFormat format);
    Texture2D(const std::filesystem::path& path);

    void Load() override;
    void Unload() override;

    void LoadImageFromMemory(uint8_t* data, uint64_t size);
    void LoadImageFromFile(const std::filesystem::path& path);
private:
    void CreateImageView(VkFormat format);
    
};

}