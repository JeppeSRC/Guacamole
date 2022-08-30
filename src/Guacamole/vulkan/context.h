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

#include "device.h"
#include "commandbuffer.h"


namespace Guacamole {

class Context {
public:
    static void Init();
    static void Shutdown();

    static VkInstance GetInstance() { return mInstance; }
    static VkPhysicalDevice GetPhysicalDeviceHandle() { return mSelectedPhysDevice->GetHandle(); }
    static PhysicalDevice* GetPhysicalDevice() { return mSelectedPhysDevice; }
    static VkDevice GetDeviceHandle() { return mLogicalDevice->GetHandle(); }
    static Device* GetDevice() { return mLogicalDevice; }
    static CommandBuffer* GetAuxCmdBuffer() { return mAuxCmdBuffer; }
private:

    struct InstanceLayer {
        VkLayerProperties Prop;
        std::vector<VkExtensionProperties> Extensions;
    };

    static std::vector<InstanceLayer> mInstanceLayers;

    static VkInstance mInstance;
    static PhysicalDevice* mSelectedPhysDevice;
    static Device* mLogicalDevice;

    static VkCommandPool mCommandPool;
    static CommandBuffer* mAuxCmdBuffer;

    static void EnumerateLayersAndExtensions();
    static bool IsLayerSupported(const char* layerName);
    static bool IsExtensionSupported(const char* extentionName);
    static uint32_t IsLayerExtensionSupported(const char* layerName, const char* extensionName);
    
};


}