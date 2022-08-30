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
#include <Guacamole/core/Window.h>
#include "commandbuffer.h"

namespace Guacamole {

class Swapchain {
public:
    static void Init(Window* window);
    static void Shutdown();

    static void Begin();
    static void Present();
    static void WaitForAllCommandBufferFences();

    static CommandBuffer* GetPrimaryCommandBuffer(uint32_t threadID);

    static VkFormat GetFormat() { return msInfo.imageFormat; }
    static VkExtent2D GetExtent() { return msInfo.imageExtent; }
    static VkQueue GetGraphicsQueue() { return mGraphicsQueue; }

    static std::vector<VkImageView> GetImageViews() { return mSwapchainImageViews; }
    static uint32_t GetCurrentImageIndex() { return mCurrentImageIndex; }

private:
    static VkSwapchainCreateInfoKHR msInfo;

    static VkSwapchainKHR mSwapchainHandle;
    static VkSurfaceKHR mSurfaceHandle;

    static VkQueue mGraphicsQueue;

    static uint32_t mCurrentImageIndex;
    static VkSemaphore mImageSemaphore;
    static VkSemaphore mSubmitSemaphore;
   
    static std::vector<std::pair<CommandPool*, std::vector<CommandBuffer*>>> mCommandPools;

    static VkSubmitInfo mSubmitInfo;
    static VkPresentInfoKHR mPresentInfo;

    static std::vector<VkImage> mSwapchainImages;
    static std::vector<VkImageView> mSwapchainImageViews;
};

}