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

std::vector<PhysicalDevice*> PhysicalDevice::physicalDevices;
uint32_t PhysicalDevice::deviceCount = 0;

void PhysicalDevice::EnumeratePhysicalDevices(VkInstance instance) {
    GM_ASSERT(instance != nullptr)

    uint32_t num = 0;

    VK(vkEnumeratePhysicalDevices(instance, &num, nullptr));
    VkPhysicalDevice* devices = new VkPhysicalDevice[num];
    VK(vkEnumeratePhysicalDevices(instance, &num, devices));

    for (uint32_t i = 0; i < num; i++) {
        physicalDevices.push_back(new PhysicalDevice(devices[i]));
    }

    delete devices;
}

PhysicalDevice* PhysicalDevice::SelectDevice() {
    for (PhysicalDevice* dev : physicalDevices) {
        uint32_t index = dev->GetPresentationQueueIndex();

        if (index != ~0) return dev;
    }

    GM_LOG_CRITICAL("No Physical device with presentation support exist");

    return nullptr;
}
    
PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) : device(device) {
    properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties(device, &properties.properties);

    uint32_t major = VK_API_VERSION_MAJOR(properties.properties.apiVersion);
    uint32_t minor = VK_API_VERSION_MINOR(properties.properties.apiVersion);

    if (major >= 1 && minor >= 1) {
        vkGetPhysicalDeviceProperties2(device, &properties);
    }

    uint32_t queueCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
    queueProperties.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueProperties.data());

    id = deviceCount++;
}

PhysicalDevice::~PhysicalDevice() {
}

uint32_t PhysicalDevice::GetPresentationQueueIndex() const {
    for (uint32_t i = 0; i < queueProperties.size(); i++) {
        const VkQueueFamilyProperties& q = queueProperties[i];

        if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT && q.queueCount > 0) {
            if (GetQueuePresentationSupport(i)) return i;
        }
    }

    return ~0;
}

bool PhysicalDevice::GetQueuePresentationSupport(uint32_t queue) const {
    return glfwGetPhysicalDevicePresentationSupport(Context::GetInstance(), device, queue) == GLFW_TRUE;
}

void PhysicalDevice::PrintDeviceInfo() const {
    GM_LOG_DEBUG("Physical Device ({4}): {5} {0} {1}.{2}.{3}", 
    properties.properties.deviceName, 
    VK_API_VERSION_MAJOR(properties.properties.apiVersion), 
    VK_API_VERSION_MINOR(properties.properties.apiVersion),
    VK_API_VERSION_PATCH(properties.properties.apiVersion),
    id, Util::vkEnumToString(properties.properties.deviceType));
}

}