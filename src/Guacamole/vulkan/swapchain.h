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

class CommandBuffer;
struct SwapchainPresentInfo {
    CommandBuffer* mCommandBuffer;
};

class Swapchain {
private:
    Swapchain(const SwapchainSpec& spec);
public:
    ~Swapchain();

    void Begin();
    void Present(SwapchainPresentInfo* presentInfo);
    VkFormat GetFormat() { return msInfo.imageFormat; }
    VkExtent2D GetExtent() { return msInfo.imageExtent; }
    std::vector<VkImageView> GetImageViews() { return mSwapchainImageViews; }
    uint32_t GetCurrentImageIndex() { return mCurrentImageIndex; }
    uint32_t GetFramesInFlight() { return mSwapchainImages.size(); }

private:
    Window* mWindow;
    Device* mDevice;
    VkSwapchainCreateInfoKHR msInfo;
    VkSwapchainKHR mSwapchainHandle;

    VkSurfaceKHR mSurfaceHandle;

    uint32_t mCurrentImageIndex;

    VkSemaphore mImageSemaphore;
    VkSemaphore mRenderSubmitSemaphore;

    CircularSemaphorePool mSemaphores;

    VkSemaphore mAuxSemaphores[SWAPCHAIN_AUX_SEMAPHORES];
    VkSubmitInfo mRenderSubmitInfo;
    VkPresentInfoKHR mPresentInfo;
    std::vector<VkImage> mSwapchainImages;
    std::vector<VkImageView> mSwapchainImageViews;

public:
    static Swapchain* CreateNew(const SwapchainSpec& spec);
    static void DestroySwapchain(Swapchain* swapchain);
    static void Shutdown();

private:
    static std::vector<Swapchain*> mSwapchains;
};

}