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

#include "device.h"

#include <Guacamole/util/util.h>
#include <GLFW/glfw3.h>
#include "context.h"

namespace Guacamole {

std::vector<PhysicalDevice*> PhysicalDevice::PhysicalDevices;
uint32_t PhysicalDevice::DeviceCount = 0;

void PhysicalDevice::EnumeratePhysicalDevices(VkInstance instance) {
    GM_ASSERT(instance != nullptr)

    uint32_t num = 0;

    VK(vkEnumeratePhysicalDevices(instance, &num, nullptr));
    VkPhysicalDevice* devices = new VkPhysicalDevice[num];
    VK(vkEnumeratePhysicalDevices(instance, &num, devices));

    for (uint32_t i = 0; i < num; i++) {
        PhysicalDevices.push_back(new PhysicalDevice(devices[i]));
    }

    delete devices;
}

PhysicalDevice* PhysicalDevice::SelectDevice() {
    for (PhysicalDevice* dev : PhysicalDevices) {
        if (dev->GetDevicePresentationSupport()) return dev;
    }

    GM_LOG_CRITICAL("No Physical device with presentation support exist");

    return nullptr;
}
    
PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) : DeviceHandle(device) {
    Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    Properties.pNext = nullptr;
    vkGetPhysicalDeviceProperties(device, &Properties.properties);

    uint32_t major = VK_API_VERSION_MAJOR(Properties.properties.apiVersion);
    uint32_t minor = VK_API_VERSION_MINOR(Properties.properties.apiVersion);

    if (major >= 1 && minor >= 1) {
        vkGetPhysicalDeviceProperties2(device, &Properties);
    }

    uint32_t queueCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
    QueueProperties.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, QueueProperties.data());

    ID = DeviceCount++;


    uint32_t extensionCount = 0;

    vkEnumerateDeviceExtensionProperties(DeviceHandle, nullptr, &extensionCount, nullptr);
    Extensions.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(DeviceHandle, nullptr, &extensionCount, Extensions.data());


}

PhysicalDevice::~PhysicalDevice() {
}

uint32_t PhysicalDevice::GetQueueIndex(VkQueueFlags queues) const {
    for (uint32_t i = 0; i < QueueProperties.size(); i++) {
        const VkQueueFamilyProperties& q = QueueProperties[i];
        
        if (q.queueFlags & queues) return i;
    }

    return ~0;
}

bool PhysicalDevice::GetDevicePresentationSupport() const {
    uint32_t index = GetQueueIndex(VK_QUEUE_GRAPHICS_BIT);

    if (index == ~0) return false;

    return GetQueuePresentationSupport(index);
}

bool PhysicalDevice::GetQueuePresentationSupport(uint32_t queueIndex) const {
    return glfwGetPhysicalDevicePresentationSupport(Context::GetInstance(), DeviceHandle, queueIndex) == GLFW_TRUE;
}

bool PhysicalDevice::IsExtensionSupported(const char* extension) const {
    GM_ASSERT(extension != nullptr)

    for (VkExtensionProperties prop : Extensions) {
        if (memcmp(prop.extensionName, extension, strlen(extension)) == 0) return true;
    }

    return false;
} 

void PhysicalDevice::PrintDeviceInfo(bool withExtensions) const {
    GM_LOG_DEBUG("Physical Device ({4}): {5} {0} {1}.{2}.{3}", 
    Properties.properties.deviceName, 
    VK_API_VERSION_MAJOR(Properties.properties.apiVersion), 
    VK_API_VERSION_MINOR(Properties.properties.apiVersion),
    VK_API_VERSION_PATCH(Properties.properties.apiVersion),
    ID, Util::vkEnumToString(Properties.properties.deviceType));


    if (withExtensions) {
        GM_LOG_DEBUG("Extensions ({0}):", Extensions.size());

        for (VkExtensionProperties ext : Extensions) {
            GM_LOG_DEBUG("\t{0}", ext.extensionName);
        }
    }
}

Device::Device(PhysicalDevice* physicalDevice, VkPhysicalDeviceFeatures features) : Parent(physicalDevice) {
    GM_ASSERT(physicalDevice != nullptr)

    std::vector<VkDeviceQueueCreateInfo> queues;

    float defaultPriority = 0.0f;
    uint32_t gQueueIndex = physicalDevice->GetQueueIndex(VK_QUEUE_GRAPHICS_BIT);
    //uint32_t cQueueIndex = physicalDevice->GetQueueIndex(VK_QUEUE_COMPUTE_BIT);
    uint32_t tQueueIndex = physicalDevice->GetQueueIndex(VK_QUEUE_TRANSFER_BIT);

    VkDeviceQueueCreateInfo queue;

    queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue.pNext = nullptr;
    queue.flags = 0;
    queue.queueCount = 1;
    queue.queueFamilyIndex = gQueueIndex;
    queue.pQueuePriorities = &defaultPriority;

    queues.push_back(queue);

    if (tQueueIndex != gQueueIndex) {
        queue.queueFamilyIndex = tQueueIndex;
        queues.push_back(queue);
    }

    const char* extensions[] = {
        "VK_KHR_swapchain"
    };

    if (!physicalDevice->IsExtensionSupported(extensions[0])) {
        GM_LOG_CRITICAL("\"{0}\" does not support {1}:", physicalDevice->GetProperties().deviceName, extensions[0]);
    }

    VkDeviceCreateInfo info;

    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.queueCreateInfoCount = queues.size();
    info.pQueueCreateInfos = queues.data();
    info.enabledExtensionCount = sizeof(extensions) / 8;
    info.ppEnabledExtensionNames = extensions;
    info.pEnabledFeatures = &features;
    info.ppEnabledLayerNames = 0;
    info.enabledLayerCount = 0;

    VK(vkCreateDevice(physicalDevice->GetHandle(), &info, nullptr, &DeviceHandle));

    GM_LOG_DEBUG("LogicalDevice created on \"{0}\"", physicalDevice->GetProperties().deviceName);

}

Device::~Device() {
    vkDestroyDevice(DeviceHandle, nullptr);
}

}