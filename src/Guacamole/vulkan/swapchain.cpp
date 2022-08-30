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

VkSwapchainCreateInfoKHR Swapchain::msInfo;
VkSwapchainKHR Swapchain::mSwapchainHandle;
VkSurfaceKHR Swapchain::mSurfaceHandle;
VkQueue Swapchain::mGraphicsQueue;
uint32_t Swapchain::mCurrentImageIndex = ~0;
VkSemaphore Swapchain::mImageSemaphore;
VkSemaphore Swapchain::mSubmitSemaphore;
std::vector<std::pair<CommandPool*, std::vector<CommandBuffer*>>> Swapchain::mCommandPools;
VkSubmitInfo Swapchain::mSubmitInfo;
VkPresentInfoKHR Swapchain::mPresentInfo;
std::vector<VkImage> Swapchain::mSwapchainImages;
std::vector<VkImageView> Swapchain::mSwapchainImageViews;

void Swapchain::Init(Window* window) {

    VK(glfwCreateWindowSurface(Context::GetInstance(), window->GetHandle(), nullptr, &mSurfaceHandle));

    msInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    msInfo.pNext = nullptr;
    msInfo.flags = 0;
    msInfo.surface = mSurfaceHandle;
    msInfo.minImageCount = 2;
    
    uint32_t formatCount = 0;

    VK(vkGetPhysicalDeviceSurfaceFormatsKHR(Context::GetPhysicalDeviceHandle(), mSurfaceHandle, &formatCount, nullptr));
    VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[formatCount];
    VK(vkGetPhysicalDeviceSurfaceFormatsKHR(Context::GetPhysicalDeviceHandle(), mSurfaceHandle, &formatCount, surfaceFormats));


    //TODO: fix
    msInfo.imageFormat = surfaceFormats[1].format;
    msInfo.imageColorSpace = surfaceFormats[1].colorSpace;

    delete surfaceFormats;

    msInfo.imageExtent.width = window->GetSpec().Width;
    msInfo.imageExtent.height = window->GetSpec().Height;
    msInfo.imageArrayLayers = 1;
    msInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    msInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    msInfo.queueFamilyIndexCount = 0;
    msInfo.pQueueFamilyIndices = nullptr;
    msInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    msInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    msInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    msInfo.clipped = VK_FALSE;
    msInfo.oldSwapchain = nullptr;

    VK(vkCreateSwapchainKHR(Context::GetDeviceHandle(), &msInfo, nullptr, &mSwapchainHandle));

    vkGetDeviceQueue(Context::GetDevice()->GetHandle(), Context::GetDevice()->GetParent()->GetQueueIndex(VK_QUEUE_GRAPHICS_BIT), 0, &mGraphicsQueue);

    uint32_t imageCount = 0;

    VK(vkGetSwapchainImagesKHR(Context::GetDeviceHandle(), mSwapchainHandle, &imageCount, nullptr));
    mSwapchainImages.resize(imageCount);
    VK(vkGetSwapchainImagesKHR(Context::GetDeviceHandle(), mSwapchainHandle, &imageCount, mSwapchainImages.data()));
    
    VkImageViewCreateInfo iwInfo;

    iwInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    iwInfo.pNext = nullptr;
    iwInfo.flags = 0;
    //iwInfo.image
    iwInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    iwInfo.format = msInfo.imageFormat;
    iwInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    iwInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    iwInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    iwInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    iwInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    iwInfo.subresourceRange.baseArrayLayer = 0;
    iwInfo.subresourceRange.baseMipLevel = 0;
    iwInfo.subresourceRange.layerCount = 1;
    iwInfo.subresourceRange.levelCount = 1;
    
    for (VkImage image : mSwapchainImages) {
        iwInfo.image = image;

        VkImageView view;

        VK(vkCreateImageView(Context::GetDeviceHandle(), &iwInfo, nullptr, &view));

        mSwapchainImageViews.push_back(view);
    }

    VkSemaphoreCreateInfo spInfo;

    spInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    spInfo.pNext = nullptr;
    spInfo.flags = 0;

    VK(vkCreateSemaphore(Context::GetDeviceHandle(), &spInfo, nullptr, &mImageSemaphore));
    VK(vkCreateSemaphore(Context::GetDeviceHandle(), &spInfo, nullptr, &mSubmitSemaphore));

     // Allocate one command pool per thread and imageCount buffers per pool
    CommandPool* mainPool = new CommandPool();
    mCommandPools.emplace_back(mainPool, mainPool->AllocateCommandBuffers(imageCount, true));

    // Preset constant values for VkSubmitInfo
    mSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    mSubmitInfo.pNext = nullptr;
    mSubmitInfo.waitSemaphoreCount = 1;
    mSubmitInfo.pWaitSemaphores = &mImageSemaphore;
    mSubmitInfo.pWaitDstStageMask = 0;
    mSubmitInfo.commandBufferCount = 1; // 1 for now
    mSubmitInfo.signalSemaphoreCount = 1;
    mSubmitInfo.pSignalSemaphores = &mSubmitSemaphore;

    // Preset constant values for VkPresentInfoKHR
    mPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    mPresentInfo.pNext = nullptr;
    mPresentInfo.waitSemaphoreCount = 1;
    mPresentInfo.pWaitSemaphores = &mSubmitSemaphore;
    mPresentInfo.swapchainCount = 1;
    mPresentInfo.pSwapchains = &mSwapchainHandle;
    
}

void Swapchain::Shutdown() {
    WaitForAllCommandBufferFences();

    vkDestroySemaphore(Context::GetDeviceHandle(), mImageSemaphore, nullptr);
    vkDestroySemaphore(Context::GetDeviceHandle(), mSubmitSemaphore, nullptr);

    for (auto& [pool, cmds] : mCommandPools) {
        delete pool;

        for (CommandBuffer* cmd : cmds)
            delete cmd;
    }

    for (VkImageView view : mSwapchainImageViews) {
        vkDestroyImageView(Context::GetDeviceHandle(), view, nullptr);
    }

    vkDestroySwapchainKHR(Context::GetDeviceHandle(), mSwapchainHandle, nullptr);
    vkDestroySurfaceKHR(Context::GetInstance(), mSurfaceHandle, nullptr);
}

void Swapchain::Begin() {
    VK(vkAcquireNextImageKHR(Context::GetDeviceHandle(), mSwapchainHandle, ~0, mImageSemaphore, VK_NULL_HANDLE, &mCurrentImageIndex));

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
   
    mSubmitInfo.pCommandBuffers = &cmdHandle;
    mSubmitInfo.pWaitDstStageMask = waitStages;


    VK(vkResetFences(Context::GetDeviceHandle(), 1, &cmdFence));
    VK(vkQueueSubmit(mGraphicsQueue, 1, &mSubmitInfo, cmdFence));

    VkResult result;

    mPresentInfo.pImageIndices = &mCurrentImageIndex;
    mPresentInfo.pResults = &result;

    VK(vkQueuePresentKHR(mGraphicsQueue, &mPresentInfo));

    mCurrentImageIndex = ~0;
}

void Swapchain::WaitForAllCommandBufferFences() {
    for (auto& [pool, cmds] : mCommandPools) {
        for (CommandBuffer* cmd : cmds) {
            cmd->WaitForFence();
        }
    }
}

CommandBuffer* Swapchain::GetPrimaryCommandBuffer(uint32_t threadID) {
    GM_ASSERT(threadID == 0);
    GM_ASSERT(mCurrentImageIndex != ~0);

    return mCommandPools[threadID].second[mCurrentImageIndex];
}

}
