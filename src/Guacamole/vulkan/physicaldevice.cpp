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

#include "physicaldevice.h"
#include "device.h"

#include <Guacamole/util/util.h>

#if defined(GM_WINDOWS)
#include <Guacamole/platform/windows/window.h>
#endif

#if defined(GM_LINUX)
#include <Guacamole/platform/linux/window.h>
#endif

namespace Guacamole {

uint32_t PhysicalDevice::mDeviceCount = 0;
    
PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) : mDeviceHandle(device) {
    mProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    mProperties.pNext = nullptr;
    vkGetPhysicalDeviceProperties(device, &mProperties.properties);

    uint32_t major = VK_API_VERSION_MAJOR(mProperties.properties.apiVersion);
    uint32_t minor = VK_API_VERSION_MINOR(mProperties.properties.apiVersion);
    
    uint32_t version = VK_MAKE_API_VERSION(0, major, minor, 0);

    if (version < GM_VK_MIN_VERSION) {
        GM_LOG_CRITICAL("[PhysicalDevice] Minimum required VK version is {}.{}, device ({}) is {}.{}", VK_VERSION_MAJOR(GM_VK_MIN_VERSION), VK_VERSION_MINOR(GM_VK_MIN_VERSION), mProperties.properties.deviceName, major, minor);
        return;
    }

    vkGetPhysicalDeviceProperties2(device, &mProperties);

    uint32_t queueCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
    mQueueProperties.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, mQueueProperties.data());

    mID = mDeviceCount++;


    uint32_t extensionCount = 0;

    vkEnumerateDeviceExtensionProperties(mDeviceHandle, nullptr, &extensionCount, nullptr);
    mExtensions.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(mDeviceHandle, nullptr, &extensionCount, mExtensions.data());

    vkGetPhysicalDeviceMemoryProperties(mDeviceHandle, &mMemoryProperties);
}

PhysicalDevice::~PhysicalDevice() {
}

uint32_t PhysicalDevice::GetQueueIndex(VkQueueFlags queues) const {
    for (uint32_t i = 0; i < mQueueProperties.size(); i++) {
        const VkQueueFamilyProperties& q = mQueueProperties[i];
        
        if (q.queueFlags & queues) return i;
    }

    return ~0;
}

bool PhysicalDevice::GetDevicePresentationSupport(const Window* window) const {
    uint32_t index = GetQueueIndex(VK_QUEUE_GRAPHICS_BIT);

    if (index == ~0ul) return false;

    return GetQueuePresentationSupport(window, index);
}

bool PhysicalDevice::GetQueuePresentationSupport(const Window* window, uint32_t queueIndex) const {
    switch (window->GetType()) {
#if defined(GM_WINDOWS)
        case Window::Type::Windows:
            return vkGetPhysicalDeviceWin32PresentationSupportKHR(mDeviceHandle, queueIndex);
#endif
#if defined(GM_WINDOW_XCB)
        case Window::Type::XCB:
            return vkGetPhysicalDeviceXcbPresentationSupportKHR(mDeviceHandle, queueIndex, ((WindowXCB*)window)->GetXCBConnection(), ((WindowXCB*)window)->GetVisualID());    
#endif
#if defined(GM_WINDOW_WAYLAND)

#endif
        default:
            GM_ASSERT(false);
            return false;
    }
}

bool PhysicalDevice::IsExtensionSupported(const char* extension) const {
    GM_ASSERT(extension != nullptr)

    for (VkExtensionProperties prop : mExtensions) {
        if (memcmp(prop.extensionName, extension, strlen(extension)) == 0) return true;
    }

    return false;
}
bool PhysicalDevice::CheckImageFormat(VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkImageFormatProperties* prop) const {
    GM_ASSERT(prop != nullptr);

    VkResult result = vkGetPhysicalDeviceImageFormatProperties(mDeviceHandle, format, imageType, tiling, usage, 0, prop);

    if (result == VK_SUCCESS) return true;

    if (result == VK_ERROR_FORMAT_NOT_SUPPORTED) {
        return false;
    }

    VK(result);
    GM_VERIFY(result == VK_SUCCESS);

    return false;
}

VkSurfaceCapabilitiesKHR PhysicalDevice::GetSurfaceCapabilities(VkSurfaceKHR surface) const {
    VkSurfaceCapabilitiesKHR cap;

    VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDeviceHandle, surface, &cap));

    return cap;
}

std::vector<VkSurfaceFormatKHR> PhysicalDevice::GetSurfaceFormats(VkSurfaceKHR surface) const {
    uint32_t count = 0;
    VK(vkGetPhysicalDeviceSurfaceFormatsKHR(mDeviceHandle, surface, &count, nullptr));
    std::vector<VkSurfaceFormatKHR> formats(count);
    VK(vkGetPhysicalDeviceSurfaceFormatsKHR(mDeviceHandle, surface, &count, formats.data()));

    return formats;
}

std::vector<VkPresentModeKHR> PhysicalDevice::GetPresentModes(VkSurfaceKHR surface) const {
    uint32_t count = 0;
    VK(vkGetPhysicalDeviceSurfacePresentModesKHR(mDeviceHandle, surface, &count, nullptr));
    std::vector<VkPresentModeKHR> modes(count);
    VK(vkGetPhysicalDeviceSurfacePresentModesKHR(mDeviceHandle, surface, &count, modes.data()));

    return modes;
}

bool PhysicalDevice::IsFeatureSupported(uint32_t feature) {
    VkPhysicalDeviceFeatures2 f2;
    VkPhysicalDeviceVulkan12Features f12;

    f2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    f2.pNext = nullptr;

    f12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    f12.pNext = nullptr;

    switch (feature)
    {
    case Device::FeatureTimelineSemaphore:
        f2.pNext = &f12;
        break;
    }

    vkGetPhysicalDeviceFeatures2(mDeviceHandle, &f2);

    switch (feature) {
        case Device::FeatureTimelineSemaphore:
            return f12.timelineSemaphore;
            break;
        case Device::FeatureAnisotropicSampling:
            return f2.features.samplerAnisotropy;
    }

    return false;
}

void PhysicalDevice::PrintDeviceInfo(bool withExtensions) const {
    GM_LOG_DEBUG("Physical Device ({4}): {5} {0} {1}.{2}.{3}", 
    mProperties.properties.deviceName, 
    VK_API_VERSION_MAJOR(mProperties.properties.apiVersion), 
    VK_API_VERSION_MINOR(mProperties.properties.apiVersion),
    VK_API_VERSION_PATCH(mProperties.properties.apiVersion),
    mID, Util::vkEnumToString(mProperties.properties.deviceType));


    if (withExtensions) {
        GM_LOG_DEBUG("Extensions ({0}):", mExtensions.size());

        for (VkExtensionProperties ext : mExtensions) {
            GM_LOG_DEBUG("\t{0}", ext.extensionName);
        }
    }
}

}