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
#include "Window.h"
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

    static VkFormat GetFormat() { return sInfo.imageFormat; }
    static VkExtent2D GetExtent() { return sInfo.imageExtent; }
    static VkQueue GetGraphicsQueue() { return GraphicsQueue; }

    static std::vector<VkImageView> GetImageViews() { return SwapchainImageViews; }
    static uint32_t GetCurrentImageIndex() { return CurrentImageIndex; }

private:
    static VkSwapchainCreateInfoKHR sInfo;

    static VkSwapchainKHR SwapchainHandle;
    static VkSurfaceKHR SurfaceHandle;

    static VkQueue GraphicsQueue;

    static uint32_t CurrentImageIndex;
    static VkSemaphore ImageSemaphore;
    static VkSemaphore SubmitSemaphore;
   
    static std::vector<std::pair<CommandPool*, std::vector<CommandBuffer*>>> CommandPools;

    static VkSubmitInfo SubmitInfo;
    static VkPresentInfoKHR PresentInfo;

    static std::vector<VkImage> SwapchainImages;
    static std::vector<VkImageView> SwapchainImageViews;
};

}