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

#include "swapchain.h"
#include "context.h"

namespace Guacamole {

VkSwapchainCreateInfoKHR Swapchain::sInfo;
VkSwapchainKHR Swapchain::SwapchainHandle;
VkSurfaceKHR Swapchain::SurfaceHandle;
VkQueue Swapchain::GraphicsQueue;
std::vector<VkImage> Swapchain::SwapchainImages;

void Swapchain::Init(Window* window) {

    VK(glfwCreateWindowSurface(Context::GetInstance(), window->GetHandle(), nullptr, &SurfaceHandle));

    sInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    sInfo.pNext = nullptr;
    sInfo.flags = 0;
    sInfo.surface = SurfaceHandle;
    sInfo.minImageCount = 2;
    
    uint32_t formatCount = 0;

    VK(vkGetPhysicalDeviceSurfaceFormatsKHR(Context::GetPhysicalDeviceHandle(), SurfaceHandle, &formatCount, nullptr));
    VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[formatCount];
    VK(vkGetPhysicalDeviceSurfaceFormatsKHR(Context::GetPhysicalDeviceHandle(), SurfaceHandle, &formatCount, surfaceFormats));

    sInfo.imageFormat = surfaceFormats[1].format;
    sInfo.imageColorSpace = surfaceFormats[1].colorSpace;

    delete surfaceFormats;

    sInfo.imageExtent.width = window->GetSpec().Width;
    sInfo.imageExtent.height = window->GetSpec().Height;
    sInfo.imageArrayLayers = 1;
    sInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    sInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    sInfo.queueFamilyIndexCount = 0;
    sInfo.pQueueFamilyIndices = nullptr;
    sInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    sInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    sInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    sInfo.clipped = VK_FALSE;
    sInfo.oldSwapchain = nullptr;

    VK(vkCreateSwapchainKHR(Context::GetDeviceHandle(), &sInfo, nullptr, &SwapchainHandle));

    uint32_t imageCount = 0;

    VK(vkGetSwapchainImagesKHR(Context::GetDeviceHandle(), SwapchainHandle, &imageCount, nullptr));
    SwapchainImages.resize(imageCount);
    VK(vkGetSwapchainImagesKHR(Context::GetDeviceHandle(), SwapchainHandle, &imageCount, SwapchainImages.data()));

    Device* dev = Context::GetDevice();

    VK(vkGetDeviceQueue(dev->GetHandle(), dev->GetParent()->GetQueueIndex(VK_QUEUE_GRAPHICS_BIT), 0, &GraphicsQueue));

}

void Swapchain::Shutdown() {
    vkDestroySwapchainKHR(Context::GetDeviceHandle(), SwapchainHandle, nullptr);
    vkDestroySurfaceKHR(Context::GetInstance(), SurfaceHandle, nullptr);
}



}
