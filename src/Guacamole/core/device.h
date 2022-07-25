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
    static std::vector<PhysicalDevice*> physicalDevices;

    static void EnumeratePhysicalDevices(VkInstance instance);
    // ATM this will select the first device with presentation support
    static PhysicalDevice* SelectDevice();
public:
    PhysicalDevice(VkPhysicalDevice device);
    ~PhysicalDevice();

    inline VkPhysicalDevice GetDevice() const { return device; }            
    inline VkPhysicalDeviceProperties GetProperties() const { return properties.properties; }

    uint32_t GetPresentationQueueIndex() const;
    bool GetQueuePresentationSupport(uint32_t queue) const;

    void PrintDeviceInfo() const;

private:
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties2 properties;
    std::vector<VkQueueFamilyProperties> queueProperties;

    uint32_t id;
    static uint32_t deviceCount;
};

}