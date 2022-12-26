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

namespace Guacamole {

class Device;
class Framebuffer {
protected:
    Device* mDevice;
    VkFramebufferCreateInfo mfbInfo;
    VkFramebuffer mFramebufferHandle;
    VkRenderPass mRenderpassHandle;

    uint32_t mWidth;
    uint32_t mHeight;

public:
    Framebuffer(Device* device, uint32_t width, uint32_t height, VkRenderPass renderpassHandle, VkImageView imageView);
    ~Framebuffer();

    void ReCreate(uint32_t width, uint32_t height, VkImageView imageView);

    inline Device* GetDevice() const { return mDevice; }
    inline VkFramebuffer GetHandle() const { return mFramebufferHandle; }
    inline VkRenderPass GetRenderpassHandle() const { return mRenderpassHandle; }
    inline uint32_t GetWidth() const { return mWidth; }
    inline uint32_t GetHeight() const { return mHeight; }

};


}