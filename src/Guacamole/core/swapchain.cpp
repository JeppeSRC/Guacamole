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

    vkGetDeviceQueue(Context::GetDevice()->GetHandle(), Context::GetDevice()->GetParent()->GetQueueIndex(VK_QUEUE_GRAPHICS_BIT), 0, &GraphicsQueue);

    uint32_t imageCount = 0;

    VK(vkGetSwapchainImagesKHR(Context::GetDeviceHandle(), SwapchainHandle, &imageCount, nullptr));
    SwapchainImages.resize(imageCount);
    VK(vkGetSwapchainImagesKHR(Context::GetDeviceHandle(), SwapchainHandle, &imageCount, SwapchainImages.data()));

    TransitionSwapchainImages();
}

void Swapchain::Shutdown() {
    vkDestroySwapchainKHR(Context::GetDeviceHandle(), SwapchainHandle, nullptr);
    vkDestroySurfaceKHR(Context::GetInstance(), SurfaceHandle, nullptr);
}

void Swapchain::TransitionSwapchainImages() {
    GM_ASSERT(SwapchainImages.size())

    VkCommandBufferBeginInfo bInfo;

    bInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bInfo.pNext = nullptr;
    bInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    bInfo.pInheritanceInfo = nullptr;

    VkCommandBuffer cmdBuffer = Context::GetAuxCmdBuffer();

    VK(vkBeginCommandBuffer(cmdBuffer, &bInfo));

    VkImageMemoryBarrier barrier[8];

    barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier[0].pNext = nullptr;
    barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier[0].srcAccessMask = 0;
    barrier[0].dstAccessMask = 0;
    barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier[0].subresourceRange.layerCount = 1;
    barrier[0].subresourceRange.baseArrayLayer = 0;
    barrier[0].subresourceRange.baseMipLevel = 0;
    barrier[0].subresourceRange.levelCount = 1;
    barrier[0].image = SwapchainImages[0];
    

    for (uint64_t i = 1; i < SwapchainImages.size(); i++) {
        memcpy(&barrier[i], barrier, sizeof(VkImageMemoryBarrier));
        barrier[i].image = SwapchainImages[i];
    }

    vkCmdPipelineBarrier(cmdBuffer, 
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 
        0, nullptr, 0, nullptr, SwapchainImages.size(), barrier);


    VK(vkEndCommandBuffer(cmdBuffer));

    VkSubmitInfo subInfo;

    subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    subInfo.pNext = nullptr;
    subInfo.commandBufferCount = 1;
    subInfo.pCommandBuffers = &cmdBuffer;
    subInfo.signalSemaphoreCount = 0;
    subInfo.pSignalSemaphores = nullptr;
    subInfo.pWaitDstStageMask = nullptr;
    subInfo.waitSemaphoreCount = 0;
    subInfo.pWaitDstStageMask = nullptr;

    VK(vkQueueSubmit(GraphicsQueue, 1, &subInfo, VK_NULL_HANDLE));
    VK(vkQueueWaitIdle(GraphicsQueue));
}

}
