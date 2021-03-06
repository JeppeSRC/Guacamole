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

#pragma once

#include <Guacamole.h>

namespace Guacamole {

class PhysicalDevice {
public:
    static std::vector<PhysicalDevice*> PhysicalDevices;

    static void EnumeratePhysicalDevices(VkInstance instance);
    // ATM this will select the first device with presentation support
    static PhysicalDevice* SelectDevice();

public:
    PhysicalDevice(VkPhysicalDevice device);
    ~PhysicalDevice();

    inline VkPhysicalDevice GetHandle() const { return DeviceHandle; }            
    inline VkPhysicalDeviceProperties GetProperties() const { return Properties.properties; }

    uint32_t GetQueueIndex(VkQueueFlags queues) const;
    bool GetDevicePresentationSupport() const;
    bool GetQueuePresentationSupport(uint32_t queueIndex) const;
    bool IsExtensionSupported(const char* extension) const;

    void PrintDeviceInfo(bool withExtensions) const;

private:
    VkPhysicalDevice DeviceHandle;
    VkPhysicalDeviceProperties2 Properties;
    std::vector<VkQueueFamilyProperties> QueueProperties;
    std::vector<VkExtensionProperties> Extensions;

    uint32_t ID;
    static uint32_t DeviceCount;
};

class Device {
public:
    Device(PhysicalDevice* physicalDevice, VkPhysicalDeviceFeatures features);
    ~Device();

    inline VkDevice GetHandle() const { return DeviceHandle; }
    inline PhysicalDevice* GetParent() const { return Parent; }

private:
    PhysicalDevice* Parent;

    VkDevice DeviceHandle;

};

}
