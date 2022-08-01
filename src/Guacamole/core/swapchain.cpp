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
std::vector<VkImageView> Swapchain::SwapchainImageViews;

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
    sInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    sInfo.clipped = VK_FALSE;
    sInfo.oldSwapchain = nullptr;

    VK(vkCreateSwapchainKHR(Context::GetDeviceHandle(), &sInfo, nullptr, &SwapchainHandle));

    vkGetDeviceQueue(Context::GetDevice()->GetHandle(), Context::GetDevice()->GetParent()->GetQueueIndex(VK_QUEUE_GRAPHICS_BIT), 0, &GraphicsQueue);

    uint32_t imageCount = 0;

    VK(vkGetSwapchainImagesKHR(Context::GetDeviceHandle(), SwapchainHandle, &imageCount, nullptr));
    SwapchainImages.resize(imageCount);
    VK(vkGetSwapchainImagesKHR(Context::GetDeviceHandle(), SwapchainHandle, &imageCount, SwapchainImages.data()));
    
    VkImageViewCreateInfo iwInfo;

    iwInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    iwInfo.flags = 0;
    //iwInfo.image
    iwInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    iwInfo.format = sInfo.imageFormat;
    iwInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    iwInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    iwInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    iwInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    iwInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    iwInfo.subresourceRange.baseArrayLayer = 0;
    iwInfo.subresourceRange.baseMipLevel = 0;
    iwInfo.subresourceRange.layerCount = 1;
    iwInfo.subresourceRange.levelCount = 1;
    
    for (VkImage image : SwapchainImages) {
        iwInfo.image = image;

        VkImageView view;

        VK(vkCreateImageView(Context::GetDeviceHandle(), &iwInfo, nullptr, &view));

        SwapchainImageViews.push_back(view);
    }

}

void Swapchain::Shutdown() {
    for (VkImageView view : SwapchainImageViews) {
        vkDestroyImageView(Context::GetDeviceHandle(), view, nullptr);
    }

    vkDestroySwapchainKHR(Context::GetDeviceHandle(), SwapchainHandle, nullptr);
    vkDestroySurfaceKHR(Context::GetInstance(), SurfaceHandle, nullptr);
}

}
