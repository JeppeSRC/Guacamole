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
#include "swapchain.h"
#include "context.h"

namespace Guacamole {

VkSwapchainCreateInfoKHR Swapchain::sInfo;
VkSwapchainKHR Swapchain::SwapchainHandle;
VkSurfaceKHR Swapchain::SurfaceHandle;
VkQueue Swapchain::GraphicsQueue;
uint32_t Swapchain::CurrentImageIndex = ~0;
VkSemaphore Swapchain::ImageSemaphore;
VkSemaphore Swapchain::SubmitSemaphore;
std::vector<std::pair<CommandPool*, std::vector<CommandBuffer*>>> Swapchain::CommandPools;
VkSubmitInfo Swapchain::SubmitInfo;
VkPresentInfoKHR Swapchain::PresentInfo;
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


    //TODO: fix
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
    iwInfo.pNext = nullptr;
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

    VkSemaphoreCreateInfo spInfo;

    spInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    spInfo.pNext = nullptr;
    spInfo.flags = 0;

    VK(vkCreateSemaphore(Context::GetDeviceHandle(), &spInfo, nullptr, &ImageSemaphore));
    VK(vkCreateSemaphore(Context::GetDeviceHandle(), &spInfo, nullptr, &SubmitSemaphore));

     // Allocate one command pool per thread and imageCount buffers per pool
    CommandPool* mainPool = new CommandPool();
    CommandPools.emplace_back(mainPool, mainPool->AllocateCommandBuffers(imageCount, true));

    // Preset constant values for VkSubmitInfo
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.pNext = nullptr;
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = &ImageSemaphore;
    SubmitInfo.pWaitDstStageMask = 0;
    SubmitInfo.commandBufferCount = 1; // 1 for now
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &SubmitSemaphore;

    // Preset constant values for VkPresentInfoKHR
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.pNext = nullptr;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &SubmitSemaphore;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &SwapchainHandle;
    
}

void Swapchain::Shutdown() {
    WaitForAllCommandBufferFences();

    vkDestroySemaphore(Context::GetDeviceHandle(), ImageSemaphore, nullptr);
    vkDestroySemaphore(Context::GetDeviceHandle(), SubmitSemaphore, nullptr);

    for (auto& [pool, cmds] : CommandPools) {
        delete pool;

        for (CommandBuffer* cmd : cmds)
            delete cmd;
    }

    for (VkImageView view : SwapchainImageViews) {
        vkDestroyImageView(Context::GetDeviceHandle(), view, nullptr);
    }

    vkDestroySwapchainKHR(Context::GetDeviceHandle(), SwapchainHandle, nullptr);
    vkDestroySurfaceKHR(Context::GetInstance(), SurfaceHandle, nullptr);
}

void Swapchain::Begin() {
    VK(vkAcquireNextImageKHR(Context::GetDeviceHandle(), SwapchainHandle, ~0, ImageSemaphore, VK_NULL_HANDLE, &CurrentImageIndex));

    CommandBuffer* cmd = GetPrimaryCommandBuffer(0);
    
    cmd->WaitForFence();
    cmd->Begin(true);
}

static VkPipelineStageFlags waitStages[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

void Swapchain::Present() {
    CommandBuffer* cmd = GetPrimaryCommandBuffer(0);
    VkCommandBuffer cmdHandle = cmd->GetHandle();
    VkFence cmdFence = cmd->GetFence();

    cmd->End();
   
    SubmitInfo.pCommandBuffers = &cmdHandle;
    SubmitInfo.pWaitDstStageMask = waitStages;


    VK(vkResetFences(Context::GetDeviceHandle(), 1, &cmdFence));
    VK(vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, cmdFence));

    VkResult result;

    PresentInfo.pImageIndices = &CurrentImageIndex;
    PresentInfo.pResults = &result;

    VK(vkQueuePresentKHR(GraphicsQueue, &PresentInfo));

    CurrentImageIndex = ~0;
}

void Swapchain::WaitForAllCommandBufferFences() {
    for (auto& [pool, cmds] : CommandPools) {
        for (CommandBuffer* cmd : cmds) {
            cmd->WaitForFence();
        }
    }
}

CommandBuffer* Swapchain::GetPrimaryCommandBuffer(uint32_t threadID) {
    GM_ASSERT(threadID == 0);
    GM_ASSERT(CurrentImageIndex != ~0);

    return CommandPools[threadID].second[CurrentImageIndex];
}

}
