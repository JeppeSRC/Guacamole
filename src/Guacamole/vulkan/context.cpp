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

#include "context.h"

#include <Guacamole/util/timer.h>

namespace Guacamole {

std::vector<Context::InstanceLayer> Context::mInstanceLayers;

VkInstance Context::mInstance;
PhysicalDevice* Context::mSelectedPhysDevice;
Device* Context::mLogicalDevice;

void Context::Init(const Window* window) {
    ScopedTimer timer1("Context::Init");
    uint32_t version = 0;

    vkEnumerateInstanceVersion(&version);

    GM_ASSERT(VK_API_VERSION_MAJOR(version) == 1 && VK_API_VERSION_MINOR(version) == 3);

    EnumerateLayersAndExtensions();

    VkApplicationInfo appInfo;

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.pApplicationName = "AppName";
    appInfo.engineVersion = 0;
    appInfo.pEngineName = "Guacamole";

    VkInstanceCreateInfo instanceInfo;

    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.flags = 0;

    const char* layer = "VK_LAYER_KHRONOS_validation";

    if (IsLayerSupported(layer)) {
        instanceInfo.enabledLayerCount = 1;
        instanceInfo.ppEnabledLayerNames = &layer;
    } else {
        GM_LOG_WARNING("VK_LAYER_KHRONOS_validation not supported");
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = nullptr;
    }

    uint32_t count = 2;
#if defined(GM_LINUX)
    const char* ext[] = {"VK_KHR_surface", VK_KHR_XCB_SURFACE_EXTENSION_NAME };
#elif defined(GM_WINDOWS)
    const char* ext[] = { "VK_KHR_surface", VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
#endif

    instanceInfo.enabledExtensionCount = count;
    instanceInfo.ppEnabledExtensionNames = ext;

    VK(vkCreateInstance(&instanceInfo, nullptr, &mInstance));

    PhysicalDevice::EnumeratePhysicalDevices(mInstance);

    for (PhysicalDevice* dev : PhysicalDevice::mPhysicalDevices)
        dev->PrintDeviceInfo(false);


    mSelectedPhysDevice = PhysicalDevice::SelectDevice(window);

    VkPhysicalDeviceFeatures features;
    memset(&features, 0, sizeof(VkPhysicalDeviceFeatures));

    features.fillModeNonSolid = true;
    features.wideLines = true;
    features.samplerAnisotropy = true;

    mLogicalDevice = new Device(mSelectedPhysDevice, features);
}


void Context::Shutdown() {
    delete mLogicalDevice;
    vkDestroyInstance(mInstance, nullptr);
}

void Context::EnumerateLayersAndExtensions() {
    uint32_t num = 0;

    VK(vkEnumerateInstanceLayerProperties(&num, nullptr));
    VkLayerProperties* props = new VkLayerProperties[num+1];
    VK(vkEnumerateInstanceLayerProperties(&num, props+1));

    GM_LOG_DEBUG("Instance Layers ({0}):", num);

    memset(props, 0, sizeof(VkLayerProperties));

    for (uint32_t i = 0; i < num+1; i++) {
        VkLayerProperties& prop = props[i];

        InstanceLayer layer;

        layer.Prop = prop;

        uint32_t extnum = 0;

        VK(vkEnumerateInstanceExtensionProperties(prop.layerName, &extnum, nullptr));
        layer.Extensions.resize(extnum);
        VK(vkEnumerateInstanceExtensionProperties(prop.layerName, &extnum, layer.Extensions.data()));

        if (i == 0) {
            const char str[] = "DEFAULT_LAYER";
            size_t len = sizeof(str);

            memcpy(prop.layerName, str, len);
        }

        GM_LOG_DEBUG("\t{0} ({1}):", prop.layerName, extnum);

        for (VkExtensionProperties ext : layer.Extensions) {
            GM_LOG_DEBUG("\t\t{0}", ext.extensionName);
        }

        mInstanceLayers.push_back(std::move(layer));
    }

    delete props;
}

bool Context::IsLayerSupported(const char* layerName) {
    return IsLayerExtensionSupported(layerName, nullptr);
}

bool Context::IsExtensionSupported(const char* extensionName) {
    return IsLayerExtensionSupported(nullptr, extensionName);
}

uint32_t Context::IsLayerExtensionSupported(const char* layerName, const char* extensionName) {
    GM_ASSERT(layerName != extensionName)

    if (layerName == nullptr) {
        size_t extLength = strlen(extensionName);

        for (VkExtensionProperties ext : mInstanceLayers[0].Extensions) {
            if (memcmp(extensionName, ext.extensionName, extLength) == 0) {
                return 1;
            }
        }

        return 0;
    }

    size_t nameLength = strlen(layerName);

    for (InstanceLayer layer : mInstanceLayers) {
        if (memcmp(layerName, layer.Prop.layerName, nameLength) == 0) {
            if (extensionName == nullptr) return 1;

            size_t extLength = strlen(extensionName);

            for (VkExtensionProperties ext : layer.Extensions) {
                if (memcmp(extensionName, ext.extensionName, extLength) == 0) {
                    return 2;
                } 
            }

            return 1;
        }   
    }

    return 0;
}

}