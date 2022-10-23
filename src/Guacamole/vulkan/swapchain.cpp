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

#include <Guacamole/asset/assetmanager.h>
#include <Guacamole/util/timer.h>

namespace Guacamole {

VkSwapchainCreateInfoKHR Swapchain::msInfo;
VkSwapchainKHR Swapchain::mSwapchainHandle;
VkSurfaceKHR Swapchain::mSurfaceHandle;
VkQueue Swapchain::mGraphicsQueue;
uint32_t Swapchain::mCurrentImageIndex = ~0;
VkSemaphore Swapchain::mImageSemaphore;
VkSemaphore Swapchain::mRenderSubmitSemaphore;
VkSemaphore Swapchain::mCopySubmitSemaphore;
VkSemaphore Swapchain::mAuxSemaphores[8];
VkSubmitInfo Swapchain::mCopySubmitInfo;
VkSubmitInfo Swapchain::mRenderSubmitInfo;
VkPresentInfoKHR Swapchain::mPresentInfo;
std::vector<VkImage> Swapchain::mSwapchainImages;
std::vector<VkImageView> Swapchain::mSwapchainImageViews;

void Swapchain::Init(Window* window) {
    ScopedTimer timer1("Swapchain::Init");
#if defined(GM_LINUX)
    VkXcbSurfaceCreateInfoKHR surfaceInfo;

    surfaceInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.flags = 0;
    surfaceInfo.connection = window->GetXCBConnection();
    surfaceInfo.window = window->GetXCBWindow();

    VK(vkCreateXcbSurfaceKHR(Context::GetInstance(), &surfaceInfo, nullptr, &mSurfaceHandle));

#elif defined(GM_WINDOWS)
    VkWin32SurfaceCreateInfoKHR surfaceInfo;

    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.flags = 0;
    surfaceInfo.hinstance = 0;
    surfaceInfo.hwnd = window->GetHWND();

    VK(vkCreateWin32SurfaceKHR(Context::GetInstance(), &surfaceInfo, nullptr, &mSurfaceHandle));
#endif

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
    
    CommandPoolManager::AllocatePrimaryRenderCommandBuffers(std::this_thread::get_id(), imageCount);

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
    VK(vkCreateSemaphore(Context::GetDeviceHandle(), &spInfo, nullptr, &mCopySubmitSemaphore));

    for (uint32_t i = 0; i < 8; i++) {
        VK(vkCreateSemaphore(Context::GetDeviceHandle(), &spInfo, nullptr, &mAuxSemaphores[i]));
    }

    mCopySubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    mCopySubmitInfo.pNext = nullptr;
    mCopySubmitInfo.waitSemaphoreCount = 0;
    mCopySubmitInfo.pWaitSemaphores = nullptr;
    mCopySubmitInfo.pWaitDstStageMask = 0;
    mCopySubmitInfo.commandBufferCount = 1; // 1 for now
    mCopySubmitInfo.signalSemaphoreCount = 1;
    mCopySubmitInfo.pSignalSemaphores = &mCopySubmitSemaphore;

    // Render submit constant values for VkSubmitInfo
    mRenderSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    mRenderSubmitInfo.pNext = nullptr;
    //mRenderSubmitInfo.waitSemaphoreCount = 2;
    //mRenderSubmitInfo.pWaitSemaphores = mAuxRenderSemaphores;
    //mRenderSubmitInfo.pWaitDstStageMask = mRenderStageFlags;
    mRenderSubmitInfo.commandBufferCount = 1; // 1 for now
    mRenderSubmitInfo.signalSemaphoreCount = 1;
    mRenderSubmitInfo.pSignalSemaphores = &mRenderSubmitSemaphore;

    // Present constant values for VkPresentInfoKHR
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
    vkDestroySemaphore(Context::GetDeviceHandle(), mCopySubmitSemaphore, nullptr);

    for (uint32_t i = 0; i < 8; i++) {
        vkDestroySemaphore(Context::GetDeviceHandle(), mAuxSemaphores[i], nullptr);
    }

    for (VkImageView view : mSwapchainImageViews) {
        vkDestroyImageView(Context::GetDeviceHandle(), view, nullptr);
    }

    vkDestroySwapchainKHR(Context::GetDeviceHandle(), mSwapchainHandle, nullptr);
    vkDestroySurfaceKHR(Context::GetInstance(), mSurfaceHandle, nullptr);
}

void Swapchain::Begin() {
    VK(vkAcquireNextImageKHR(Context::GetDeviceHandle(), mSwapchainHandle, ~0, mImageSemaphore, VK_NULL_HANDLE, &mCurrentImageIndex));

    CommandBuffer* cmd = GetPrimaryCommandBuffer();
    CommandBuffer* aux = CommandPoolManager::GetCopyCommandBuffer();
    
    // This is temporary
    aux->WaitForFence();
    aux->Begin(true);

    cmd->WaitForFence();
    cmd->Begin(true);
}

void Swapchain::Present() {
    mRenderSubmitInfo.waitSemaphoreCount = 1;

    std::vector<VkSemaphore> waitSemaphores({ mImageSemaphore });
    std::vector<VkPipelineStageFlags> renderStageFlags({VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT});

    std::vector<AssetManager::FinishedAsset> finishedAssets = AssetManager::GetFinishedAssets();

    uint32_t semaphoreIndex = 0;
    
    for (AssetManager::FinishedAsset& asset : finishedAssets) {
        VkSubmitInfo info;

        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = nullptr;
        info.waitSemaphoreCount = 0;
        info.pWaitSemaphores = nullptr;
        info.pWaitDstStageMask = nullptr;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &asset.mCommandBuffer->GetHandle();
        info.signalSemaphoreCount = 0;
        info.pSignalSemaphores = nullptr;
        //info.signalSemaphoreCount = 1;
        //info.pSignalSemaphores = &mAuxSemaphores[semaphoreIndex];

        //waitSemaphores.push_back(mAuxSemaphores[semaphoreIndex++]);
        //renderStageFlags.push_back(VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);

        VK(vkResetFences(Context::GetDeviceHandle(), 1, &asset.mCommandBuffer->GetFence()));
        VK(vkQueueSubmit(mGraphicsQueue, 1, &info, asset.mCommandBuffer->GetFence()));
    }

    VK(vkQueueWaitIdle(mGraphicsQueue));

    for (AssetManager::FinishedAsset& asset : finishedAssets) {
        asset.mAsset->Unmap();
    }

    CommandBuffer* cmd = CommandPoolManager::GetCopyCommandBuffer();

    if (cmd->IsUsed()) {
        cmd->End();
        mCopySubmitInfo.pCommandBuffers = &cmd->GetHandle();

        VK(vkResetFences(Context::GetDeviceHandle(), 1, &cmd->GetFence()));
        VK(vkQueueSubmit(mGraphicsQueue, 1, &mCopySubmitInfo, cmd->GetFence()));

        waitSemaphores.push_back(mCopySubmitSemaphore);
        renderStageFlags.push_back(VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);

        mRenderSubmitInfo.waitSemaphoreCount++;
    }

    cmd = GetPrimaryCommandBuffer();
    cmd->End();
   
    mRenderSubmitInfo.waitSemaphoreCount = waitSemaphores.size();
    mRenderSubmitInfo.pWaitSemaphores = waitSemaphores.data();
    mRenderSubmitInfo.pWaitDstStageMask = renderStageFlags.data();
    mRenderSubmitInfo.pCommandBuffers = &cmd->GetHandle();

    VK(vkResetFences(Context::GetDeviceHandle(), 1, &cmd->GetFence()));
    VK(vkQueueSubmit(mGraphicsQueue, 1, &mRenderSubmitInfo, cmd->GetFence()));

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
    return CommandPoolManager::GetRenderCommandBuffer();
}

}
