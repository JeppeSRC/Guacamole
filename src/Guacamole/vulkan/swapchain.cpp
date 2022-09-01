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
VkSemaphore Swapchain::mRenderSubmitSemaphore;
VkSemaphore Swapchain::mAuxSubmitSemaphore;
VkSemaphore Swapchain::mAuxRenderSemaphores[2];
VkPipelineStageFlags Swapchain::mRenderStageFlags[2];
VkSubmitInfo Swapchain::mAuxSubmitInfo;
VkSubmitInfo Swapchain::mRenderSubmitInfo;
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
    
    CommandPoolManager::AllocatePrimaryRenderCommandBuffers(imageCount);

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
    VK(vkCreateSemaphore(Context::GetDeviceHandle(), &spInfo, nullptr, &mRenderSubmitSemaphore));
    VK(vkCreateSemaphore(Context::GetDeviceHandle(), &spInfo, nullptr, &mAuxSubmitSemaphore));

    // Aux submit constant values for VkSubmitInfo
    mAuxSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    mAuxSubmitInfo.pNext = nullptr;
    mAuxSubmitInfo.waitSemaphoreCount = 0;
    mAuxSubmitInfo.pWaitSemaphores = nullptr;
    mAuxSubmitInfo.pWaitDstStageMask = 0;
    mAuxSubmitInfo.commandBufferCount = 1; // 1 for now
    mAuxSubmitInfo.signalSemaphoreCount = 1;
    mAuxSubmitInfo.pSignalSemaphores = &mAuxSubmitSemaphore;

    mAuxRenderSemaphores[0] = mImageSemaphore;
    mAuxRenderSemaphores[1] = mAuxSubmitSemaphore;

    mRenderStageFlags[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    mRenderStageFlags[1] = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;

    // Render submit constant values for VkSubmitInfo
    mRenderSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    mRenderSubmitInfo.pNext = nullptr;
    mRenderSubmitInfo.waitSemaphoreCount = 2;
    mRenderSubmitInfo.pWaitSemaphores = mAuxRenderSemaphores;
    mRenderSubmitInfo.pWaitDstStageMask = mRenderStageFlags;
    mRenderSubmitInfo.commandBufferCount = 1; // 1 for now
    mRenderSubmitInfo.signalSemaphoreCount = 1;
    mRenderSubmitInfo.pSignalSemaphores = &mRenderSubmitSemaphore;

    // Preset constant values for VkPresentInfoKHR
    mPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    mPresentInfo.pNext = nullptr;
    mPresentInfo.waitSemaphoreCount = 1;
    mPresentInfo.pWaitSemaphores = &mRenderSubmitSemaphore;
    mPresentInfo.swapchainCount = 1;
    mPresentInfo.pSwapchains = &mSwapchainHandle;
    
}

void Swapchain::Shutdown() {
    WaitForAllCommandBufferFences();

    vkDestroySemaphore(Context::GetDeviceHandle(), mImageSemaphore, nullptr);
    vkDestroySemaphore(Context::GetDeviceHandle(), mRenderSubmitSemaphore, nullptr);
    vkDestroySemaphore(Context::GetDeviceHandle(), mAuxSubmitSemaphore, nullptr);

    for (VkImageView view : mSwapchainImageViews) {
        vkDestroyImageView(Context::GetDeviceHandle(), view, nullptr);
    }

    vkDestroySwapchainKHR(Context::GetDeviceHandle(), mSwapchainHandle, nullptr);
    vkDestroySurfaceKHR(Context::GetInstance(), mSurfaceHandle, nullptr);
}

void Swapchain::Begin() {
    VK(vkAcquireNextImageKHR(Context::GetDeviceHandle(), mSwapchainHandle, ~0, mImageSemaphore, VK_NULL_HANDLE, &mCurrentImageIndex));

    CommandBuffer* cmd = GetPrimaryCommandBuffer();
    CommandBuffer* aux = CommandPoolManager::GetAuxCommandBuffer();
    
    // This is temporary
    aux->WaitForFence();
    aux->Begin(true);

    cmd->WaitForFence();
    cmd->Begin(true);
}

void Swapchain::Present() {
    CommandBuffer* cmd = CommandPoolManager::GetAuxCommandBuffer();

    if (cmd->IsUsed()) {
        VkCommandBuffer auxHandle = cmd->GetHandle();
        VkFence auxFence = cmd->GetFence();

        cmd->End();

        mAuxSubmitInfo.pCommandBuffers = &auxHandle;

        VK(vkResetFences(Context::GetDeviceHandle(), 1, &auxFence));
        VK(vkQueueSubmit(mGraphicsQueue, 1, &mAuxSubmitInfo, auxFence));

        mRenderSubmitInfo.waitSemaphoreCount = 2;
    } else {
        mRenderSubmitInfo.waitSemaphoreCount = 1;
    }
     

    cmd = GetPrimaryCommandBuffer();
    VkCommandBuffer cmdHandle = cmd->GetHandle();
    VkFence cmdFence = cmd->GetFence();

    cmd->End();
   
    mRenderSubmitInfo.pCommandBuffers = &cmdHandle;

    VK(vkResetFences(Context::GetDeviceHandle(), 1, &cmdFence));
    VK(vkQueueSubmit(mGraphicsQueue, 1, &mRenderSubmitInfo, cmdFence));

    VkResult result;

    mPresentInfo.pImageIndices = &mCurrentImageIndex;
    mPresentInfo.pResults = &result;

    VK(vkQueuePresentKHR(mGraphicsQueue, &mPresentInfo));

    mCurrentImageIndex = ~0;
}

void Swapchain::WaitForAllCommandBufferFences() {
    CommandPoolManager::WaitForRenderFences();
}

CommandBuffer* Swapchain::GetPrimaryCommandBuffer() {
    return CommandPoolManager::GetPrimaryRenderCommandBuffer();
}

}
