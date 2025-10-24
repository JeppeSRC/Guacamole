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

#include "util.h"

#include <Guacamole/core/video/window.h>
#include <Guacamole/vulkan/swapchain.h>
#include <Guacamole/vulkan/device.h>

#define SWAPCHAIN_AUX_SEMAPHORES 8

namespace Guacamole {

struct SwapchainSpec {
    Window* mWindow;
    Device* mDevice;

    std::vector<VkSurfaceFormatKHR> mPreferredFormats;
    std::vector<VkPresentModeKHR> mPreferredPresentModes;
};

class Framebuffer;
class Event;
class CommandPool;
class CommandBuffer;
class Swapchain {
private:
    Swapchain(const SwapchainSpec& spec);
public:
    ~Swapchain();

    // Returns true if an image was aquired
    bool Begin();
    void Present();
    bool Resize(uint32_t width, uint32_t height);
    VkFormat GetFormat() { return msInfo.imageFormat; }
    VkExtent2D GetExtent() { return msInfo.imageExtent; }
    std::vector<VkImageView> GetImageViews() { return mSwapchainImageViews; }
    uint32_t GetCurrentImageIndex() { return mCurrentImageIndex; }
    uint32_t GetFramesInFlight() { return (uint32_t)mSwapchainImages.size(); }
    CommandBuffer* GetRenderCommandBuffer() { return mCommandBuffers[mCurrentImageIndex]; }

    void AddFramebuffer(uint32_t viewIndex, Framebuffer* framebuffer);
    void RemoveFramebuffer(uint32_t viewIndex, Framebuffer* framebuffer);

private:
    bool OnEvent(Event* event);
private:
    Window* mWindow;
    Device* mDevice;
    VkSwapchainCreateInfoKHR msInfo;
    VkSwapchainKHR mSwapchainHandle;

#if defined(GM_LINUX)
    VkXcbSurfaceCreateInfoKHR mSurfaceInfo;
#elif defined(GM_WINDOWS)
    VkWin32SurfaceCreateInfoKHR mSurfaceInfo;
#endif

    VkSurfaceKHR mSurfaceHandle;

    uint32_t mCurrentImageIndex;

    VkSemaphore mImageSemaphore;
    VkSemaphore mRenderSubmitSemaphore;

    CircularSemaphorePool mSemaphores;

    CommandPool* mCommandPool;
    std::vector<CommandBuffer*> mCommandBuffers;

    VkPresentInfoKHR mPresentInfo;
    VkImageViewCreateInfo miwInfo;
    std::vector<VkImage> mSwapchainImages;
    std::vector<VkImageView> mSwapchainImageViews;

    std::map<uint32_t, std::vector<Framebuffer*>> mFramebuffers; // Framebuffers created from swapchain images
public:
    static Swapchain* CreateNew(const SwapchainSpec& spec);
    static void DestroySwapchain(Swapchain* swapchain);
    static void Shutdown();

private:
    static std::vector<Swapchain*> mSwapchains;
};

}