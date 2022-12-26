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
#include "framebuffer.h"

#include <Guacamole/vulkan/device.h>

namespace Guacamole {

Framebuffer::Framebuffer(Device* device, uint32_t width, uint32_t height, VkRenderPass renderpassHandle, VkImageView imageView) 
        : mDevice(device), mWidth(width), mHeight(height), mRenderpassHandle(renderpassHandle) {
            
    mfbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    mfbInfo.pNext = nullptr;
    mfbInfo.flags = 0;
    mfbInfo.renderPass = mRenderpassHandle;
    mfbInfo.attachmentCount = 1;
    mfbInfo.pAttachments = &imageView;
    mfbInfo.width = width;
    mfbInfo.height = height;
    mfbInfo.layers = 1;

    VK(vkCreateFramebuffer(device->GetHandle(), &mfbInfo, nullptr, &mFramebufferHandle));
}

Framebuffer::~Framebuffer() {
    vkDestroyFramebuffer(mDevice->GetHandle(), mFramebufferHandle, nullptr);
}

void Framebuffer::ReCreate(uint32_t width, uint32_t height, VkImageView imageView) {
    vkDestroyFramebuffer(mDevice->GetHandle(), mFramebufferHandle, nullptr);

    mfbInfo.width = width;
    mfbInfo.height = height;
    mfbInfo.pAttachments = &imageView;

    VK(vkCreateFramebuffer(mDevice->GetHandle(), &mfbInfo, nullptr, &mFramebufferHandle));

    mWidth = width;
    mHeight = height;
}

}