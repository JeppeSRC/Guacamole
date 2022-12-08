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
#include "device.h"

#include <Guacamole/asset/assetmanager.h>
#include <Guacamole/util/timer.h>
#include <Guacamole/vulkan/buffer/stagingbuffer.h>

bool operator==(const VkSurfaceFormatKHR& left, const VkSurfaceFormatKHR& right) {
    return left.colorSpace == right.colorSpace && left.format == right.format;
}

namespace Guacamole {

Swapchain::Swapchain(const SwapchainSpec& spec) : mSemaphores(spec.mDevice) {
    ScopedTimer timer1("Swapchain::Init");

    Window* window = spec.mWindow;
    mDevice = spec.mDevice;
    PhysicalDevice* physical = mDevice->GetParent();

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

    VkSurfaceCapabilitiesKHR surfaceCaps = physical->GetSurfaceCapabilities(mSurfaceHandle);

    msInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    msInfo.pNext = nullptr;
    msInfo.flags = 0;
    msInfo.surface = mSurfaceHandle;
    msInfo.minImageCount = surfaceCaps.minImageCount;

    std::vector<VkSurfaceFormatKHR> surfaceFormats = physical->GetSurfaceFormats(mSurfaceHandle);

    GM_VERIFY_MSG(surfaceFormats.size() > 0, "Selected device doesn't support any surface formats");

    msInfo.imageFormat = VK_FORMAT_UNDEFINED;

    for (VkSurfaceFormatKHR format : surfaceFormats)  {
        for (VkSurfaceFormatKHR wanted : spec.mPreferredFormats) {
            if (wanted == format) {
                msInfo.imageFormat = format.format;
                msInfo.imageColorSpace = format.colorSpace;
            }
        }
    }

    if (msInfo.imageFormat == VK_FORMAT_UNDEFINED) {
        GM_LOG_WARNING("[Swapchain] Selected device ({}) doesn't support any of the preferred surface formats, selecting first format", physical->GetProperties().deviceName);
        msInfo.imageFormat = surfaceFormats[0].format;
        msInfo.imageColorSpace = surfaceFormats[0].colorSpace;
    }

    std::vector<VkPresentModeKHR> presentModes = physical->GetPresentModes(mSurfaceHandle);

    msInfo.presentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;

    for (VkPresentModeKHR preferred : spec.mPreferredPresentModes) {
        for (VkPresentModeKHR mode : presentModes) {
            if (preferred == mode) {
                msInfo.presentMode = preferred;
            }
        }
    }

    if (msInfo.presentMode == VK_PRESENT_MODE_MAX_ENUM_KHR) {
        GM_LOG_WARNING("[Swapchain] Selected device ({}) doesn't support any of the preferred present modes, selecting VK_PRESENT_MODE_FIFO_KHR", physical->GetProperties().deviceName);
        msInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    }

    msInfo.imageExtent.width = window->GetSpec().Width;
    msInfo.imageExtent.height = window->GetSpec().Height;
    msInfo.imageArrayLayers = 1;
    msInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    msInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    msInfo.queueFamilyIndexCount = 0;
    msInfo.pQueueFamilyIndices = nullptr;
    msInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    msInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    msInfo.clipped = VK_FALSE;
    msInfo.oldSwapchain = nullptr;

    VK(vkCreateSwapchainKHR(mDevice->GetHandle(), &msInfo, nullptr, &mSwapchainHandle));

    uint32_t imageCount = 0;

    VK(vkGetSwapchainImagesKHR(mDevice->GetHandle(), mSwapchainHandle, &imageCount, nullptr));
    mSwapchainImages.resize(imageCount);
    VK(vkGetSwapchainImagesKHR(mDevice->GetHandle(), mSwapchainHandle, &imageCount, mSwapchainImages.data()));
    
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

        VK(vkCreateImageView(mDevice->GetHandle(), &iwInfo, nullptr, &view));

        mSwapchainImageViews.push_back(view);
    }

    VkSemaphoreCreateInfo spInfo;

    spInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    spInfo.pNext = nullptr;
    spInfo.flags = 0;

    mSemaphores.Init((SWAPCHAIN_AUX_SEMAPHORES + 1) * imageCount + 1);

    mCommandPool = new CommandPool(mDevice);
    mCommandBuffers = mCommandPool->AllocateCommandBuffers(imageCount, true);

    // Present constant values for VkPresentInfoKHR
    mPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    mPresentInfo.pNext = nullptr;
    mPresentInfo.waitSemaphoreCount = 1;
    mPresentInfo.pWaitSemaphores = &mRenderSubmitSemaphore;
    mPresentInfo.swapchainCount = 1;
    mPresentInfo.pSwapchains = &mSwapchainHandle;
    
}

Swapchain::~Swapchain() {
    for (CommandBuffer* cmd : mCommandBuffers) {
        delete cmd;
    }

    delete mCommandPool;

    for (VkImageView view : mSwapchainImageViews) {
        vkDestroyImageView(mDevice->GetHandle(), view, nullptr);
    }

    vkDestroySwapchainKHR(mDevice->GetHandle(), mSwapchainHandle, nullptr);
    vkDestroySurfaceKHR(Context::GetInstance(), mSurfaceHandle, nullptr);
}

bool Swapchain::Begin() {
    mImageSemaphore = mSemaphores.Get();
    mRenderSubmitSemaphore = mSemaphores.Get();
    VkResult res = vkAcquireNextImageKHR(mDevice->GetHandle(), mSwapchainHandle, 1000, mImageSemaphore, VK_NULL_HANDLE, &mCurrentImageIndex);
    
    if (res == VK_SUCCESS) return true;

    mSemaphores.Recyle(2);

    if (res == VK_TIMEOUT || res == VK_NOT_READY) {
        return false;
    }

    VK(res);

    return false;
}

void Swapchain::Present() {
    if (mDevice->GetFeatures() & Device::FeatureTimelineSemaphore) PresentInternalTimelineSemaphore();
}

void Swapchain::PresentInternalTimelineSemaphore() {
    GM_ASSERT(mDevice->GetFeatures() & Device::FeatureTimelineSemaphore);

    std::vector<VkSemaphore> renderWaitSemaphores({ mImageSemaphore });
    std::vector<VkPipelineStageFlags> renderWaitStageFlags({VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT});

    std::vector<VkSubmitInfo> submits;
    std::vector<VkSemaphore> otherSemaphores;
    std::vector<VkTimelineSemaphoreSubmitInfoKHR> semaphoreSubmits;
    std::vector<uint64_t> semaphoreWaitValues;
    semaphoreSubmits.reserve(32);
    semaphoreWaitValues.reserve(32);
    otherSemaphores.reserve(32);

    semaphoreWaitValues.push_back(0);

    { // staging buffer submission
        std::vector<StagingBufferSubmitInfo> stagingBuffers = StagingManager::GetSubmittedStagingBuffers();

        for (StagingBufferSubmitInfo& buf : stagingBuffers) {
            CommandBuffer* bufCmd = buf.mStagingBuffer->GetCommandBuffer();
            bufCmd->End();

            SemaphoreTimeline* sem = (SemaphoreTimeline*)bufCmd->GetSemaphore();

            renderWaitSemaphores.push_back(sem->GetHandle());
            renderWaitStageFlags.push_back(buf.mStageFlags);

            semaphoreWaitValues.push_back(sem->IncrementSignalCounter());

            VkTimelineSemaphoreSubmitInfoKHR& semInfo = semaphoreSubmits.emplace_back();
            semInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR;
            semInfo.pNext = nullptr;
            semInfo.waitSemaphoreValueCount = 0;
            semInfo.pWaitSemaphoreValues = nullptr;
            semInfo.signalSemaphoreValueCount = 1;
            semInfo.pSignalSemaphoreValues = &semaphoreWaitValues.back();

            VkSubmitInfo& submitInfo = submits.emplace_back();
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext = &semInfo;
            submitInfo.waitSemaphoreCount = 0;
            submitInfo.pWaitSemaphores = nullptr;
            submitInfo.pWaitDstStageMask = nullptr;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &bufCmd->GetHandle();
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &sem->GetHandle();
        }
    }
    
    CommandBuffer* cmd = GetRenderCommandBuffer();
    cmd->End();
   
    SemaphoreTimeline* sem = (SemaphoreTimeline*)cmd->GetSemaphore();
    VkSemaphore signalSemaphores[] = {mRenderSubmitSemaphore, sem->GetHandle()};

    uint64_t signalValues[] = {0, sem->IncrementSignalCounter()};

    GM_ASSERT(renderWaitSemaphores.size() == semaphoreWaitValues.size());

    VkTimelineSemaphoreSubmitInfoKHR& semInfo = semaphoreSubmits.emplace_back();
    semInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR;
    semInfo.pNext = nullptr;
    semInfo.waitSemaphoreValueCount = (uint32_t)renderWaitSemaphores.size();
    semInfo.pWaitSemaphoreValues = semaphoreWaitValues.data();
    semInfo.signalSemaphoreValueCount = 2;
    semInfo.pSignalSemaphoreValues = signalValues;

    VkSubmitInfo& renderSubmitInfo = submits.emplace_back();
    renderSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    renderSubmitInfo.pNext = &semInfo;
    renderSubmitInfo.commandBufferCount = 1;
    renderSubmitInfo.pCommandBuffers = &cmd->GetHandle();
    renderSubmitInfo.waitSemaphoreCount = semInfo.waitSemaphoreValueCount;
    renderSubmitInfo.pWaitSemaphores = renderWaitSemaphores.data();
    renderSubmitInfo.pWaitDstStageMask = renderWaitStageFlags.data();
    renderSubmitInfo.signalSemaphoreCount = 2;
    renderSubmitInfo.pSignalSemaphores = signalSemaphores;

    VK(vkQueueSubmit(mDevice->GetGraphicsQueue(), (uint32_t)submits.size(), submits.data(), nullptr));

    mPresentInfo.pImageIndices = &mCurrentImageIndex;
    mPresentInfo.pResults = nullptr;

    VK(vkQueuePresentKHR(mDevice->GetGraphicsQueue(), &mPresentInfo));

    mCurrentImageIndex = ~0;
}

std::vector<Swapchain*> Swapchain::mSwapchains;

Swapchain* Swapchain::CreateNew(const SwapchainSpec& spec) {
    Swapchain* sw = new Swapchain(spec);
    mSwapchains.push_back(sw);
    return sw;
}

void Swapchain::DestroySwapchain(Swapchain* swapchain) {
    for (uint32_t i = 0; i < mSwapchains.size(); i++) {
        if (mSwapchains[i] == swapchain) {
            delete swapchain;
            mSwapchains.erase(mSwapchains.begin()+i);
        }
    }

    GM_ASSERT(false);
}

void Swapchain::Shutdown() {
    for (Swapchain* swapchain : mSwapchains) {
        delete swapchain;
    }
}

}
