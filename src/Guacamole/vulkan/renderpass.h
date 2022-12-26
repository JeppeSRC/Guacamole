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
#include <Guacamole/vulkan/buffer/commandbuffer.h>
#include <Guacamole/vulkan/buffer/framebuffer.h>

namespace Guacamole {

class Swapchain;
class Device;
class Renderpass {
public:
    virtual ~Renderpass();

    VkRenderPass GetHandle() const { return mRenderpassHandle; }
    virtual VkFramebuffer GetFramebufferHandle(uint32_t index) const = 0;
    virtual void Begin(const CommandBuffer* cmd) = 0;
    virtual void End(const CommandBuffer* cmd) = 0;

protected:
    Renderpass(Device* device, Swapchain* swapchain) : mDevice(device), mSwapchain(swapchain) {}

    VkRenderPass mRenderpassHandle;
    VkRenderPassBeginInfo mBeginInfo;

    Device* mDevice;
    Swapchain* mSwapchain;

    void Create(VkRenderPassCreateInfo* rInfo);
private:

};

class BasicRenderpass : public Renderpass {
public:
    BasicRenderpass(Swapchain* swapchain, Device* device);
    ~BasicRenderpass();

    void Begin(const CommandBuffer* cmd) override;
    void End(const CommandBuffer* cmd) override;

    VkFramebuffer GetFramebufferHandle(uint32_t index) const override;
    
    inline uint32_t GetWidth() const { return mFramebuffers[0].GetWidth(); }
    inline uint32_t GetHeight() const { return mFramebuffers[0].GetHeight(); }

private:
    std::vector<Framebuffer> mFramebuffers;
    
};

}