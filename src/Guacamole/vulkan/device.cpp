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

#include "device.h"

#include <Guacamole/util/util.h>

namespace Guacamole {

Device::Device(PhysicalDevice* physicalDevice) : mParent(physicalDevice), mEnabledFeatures(FeatureTimelineSemaphore) {
    GM_ASSERT(physicalDevice != nullptr)

    if (!mParent->IsFeatureSupported(FeatureTimelineSemaphore)) {
        GM_ASSERT_MSG(false, "TimelineSemaphores not supported!")
    }

    std::vector<VkDeviceQueueCreateInfo> queues;

    float defaultPriority = 0.0f;
    mGraphicsQueueIndex = physicalDevice->GetQueueIndex(VK_QUEUE_GRAPHICS_BIT);
    //uint32_t cQueueIndex = physicalDevice->GetQueueIndex(VK_QUEUE_COMPUTE_BIT);
    //mTransferQueueIndex = physicalDevice->GetQueueIndex(VK_QUEUE_TRANSFER_BIT);

    VkDeviceQueueCreateInfo queue;

    queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue.pNext = nullptr;
    queue.flags = 0;
    queue.queueCount = 1;
    queue.queueFamilyIndex = mGraphicsQueueIndex;
    queue.pQueuePriorities = &defaultPriority;

    queues.push_back(queue);

    /*if (mGraphicsQueueIndex != mTransferQueueIndex) {
        queue.queueFamilyIndex = mTransferQueueIndex;
        queues.push_back(queue);
    }*/


    std::vector<const char*> extensions;

    if (!physicalDevice->IsExtensionSupported("VK_KHR_swapchain")) {
        GM_LOG_CRITICAL("\"{0}\" does not support {1}:", physicalDevice->GetProperties().deviceName, "VK_KHR_swapchain");
        GM_VERIFY(false);
    }

    extensions.push_back("VK_KHR_swapchain");

    VkPhysicalDeviceFeatures2 features2 = {};
    VkPhysicalDeviceVulkan12Features features12 = {};

    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
    features2.pNext = &features12;
    features2.features.samplerAnisotropy = mParent->IsFeatureSupported(FeatureAnisotropicSampling);

    if (features2.features.samplerAnisotropy) 
        mEnabledFeatures |= FeatureAnisotropicSampling;
    
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.pNext = nullptr;
    features12.timelineSemaphore = true;

    VkDeviceCreateInfo info;

    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = &features2;
    info.flags = 0;
    info.queueCreateInfoCount = (uint32_t)queues.size();
    info.pQueueCreateInfos = queues.data();
    info.enabledExtensionCount = (uint32_t)extensions.size();
    info.ppEnabledExtensionNames = extensions.data();
    info.pEnabledFeatures = nullptr;
    info.ppEnabledLayerNames = 0;
    info.enabledLayerCount = 0;

    VK(vkCreateDevice(physicalDevice->GetHandle(), &info, nullptr, &mDeviceHandle));

    GM_LOG_DEBUG("LogicalDevice created on \"{0}\"", physicalDevice->GetProperties().deviceName);

    vkGetDeviceQueue(mDeviceHandle, mGraphicsQueueIndex, 0, &mGraphicsQueue);
}

Device::~Device() {
    vkDestroyDevice(mDeviceHandle, nullptr);
}

void Device::WaitQueueIdle() const { 
    vkQueueWaitIdle(mGraphicsQueue);    
}


}