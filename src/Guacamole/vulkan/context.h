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

#include "physicaldevice.h"
#include "device.h"

namespace Guacamole {

struct ContextSpec {
    std::string applicationName;
};

class Context {
public:
    static void Init(const ContextSpec& spec);
    static void Shutdown();

    // ATM this will select the first device with presentation support
    static Device* CreateDevice(uint32_t deviceIndex);
    static Device* CreateDevice(const Window* window);
    static void DestroyDevice(Device* device);
    static std::vector<PhysicalDevice*> GetPhysicalDevices() { return mPhysicalDevices; }
    static std::vector<Device*> GetDevices() { return mDevices; }

    static VkInstance GetInstance() { return mInstance; }
private:

    struct InstanceLayer {
        VkLayerProperties Prop;
        std::vector<VkExtensionProperties> Extensions;
    };

    static std::vector<InstanceLayer> mInstanceLayers;
    static std::vector<PhysicalDevice*> mPhysicalDevices;
    static std::vector<Device*> mDevices;

    static VkInstance mInstance;

    static PhysicalDevice* GetFirstSupportedPhysicalDevice(const Window* window);
    static void EnumerateLayersAndExtensions();
    static bool IsLayerSupported(const char* layerName);
    static bool IsExtensionSupported(const char* extentionName);
    static uint32_t IsLayerExtensionSupported(const char* layerName, const char* extensionName);
    static void EnumeratePhysicalDevices();
    
};


}