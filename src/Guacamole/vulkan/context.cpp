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
std::vector<PhysicalDevice*> Context::mPhysicalDevices;
std::vector<Device*> Context::mDevices;

VkInstance Context::mInstance;

void Context::Init(const ContextSpec& spec) {
    ScopedTimer timer1("Context::Init");
    uint32_t version = 0;

    vkEnumerateInstanceVersion(&version);

    uint32_t minVersion = GM_VK_MIN_VERSION;

    //uint32_t a = VK_API_VERSION_VARIANT(version);
    //uint32_t major = VK_API_VERSION_MAJOR(version);
    //uint32_t minor = VK_API_VERSION_MINOR(version);

    GM_VERIFY(version >= minVersion);
    
    EnumerateLayersAndExtensions();

    VkApplicationInfo appInfo;

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.apiVersion = version;
    appInfo.pApplicationName = spec.applicationName.c_str();
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

    EnumeratePhysicalDevices();
}


void Context::Shutdown() {
    for (Device* device : mDevices) {
        GM_LOG_DEBUG("[Context] Destroyed device \"{}\"", device->GetParent()->GetProperties().deviceName);
        delete device;
    }

    mDevices.clear();

    vkDestroyInstance(mInstance, nullptr);
}

Device* Context::CreateDevice(uint32_t deviceIndex) {
    GM_VERIFY_MSG(deviceIndex < mPhysicalDevices.size(), "Invalid device index");

    for (Device* device : mDevices) {
        PhysicalDevice* dev = device->GetParent();

        if (dev->GetDeviceIndex() == deviceIndex) {
            GM_LOG_CRITICAL("[Context] Logical device has already been created on \"{}\"", dev->GetProperties().deviceName);
            GM_VERIFY(false);
        }
    }

    Device* device = new Device(mPhysicalDevices[deviceIndex]);
    mDevices.push_back(device);

    return device;
}

Device* Context::CreateDevice(const Window* window) {
    GM_ASSERT(window);

    PhysicalDevice* dev = GetFirstSupportedPhysicalDevice(window);
    GM_VERIFY(dev);

    return CreateDevice(dev->GetDeviceIndex());
}

void Context::DestroyDevice(Device* device) {
    for (uint32_t i = 0; i < mDevices.size(); i++) {
        if (mDevices[i] == device) {
            GM_LOG_DEBUG("[Context] Destroyed device \"{}\"", device->GetParent()->GetProperties().deviceName);
            delete device;
            mDevices.erase(mDevices.begin()+i);
            return;
        }
    }

    GM_ASSERT(false);
}

PhysicalDevice* Context::GetFirstSupportedPhysicalDevice(const Window* window) {
    for (PhysicalDevice* dev : mPhysicalDevices) {
        if (dev->GetDevicePresentationSupport(window)) return dev;
    }

    GM_LOG_CRITICAL("No Physical device with presentation support exist");

    return nullptr;
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

void Context::EnumeratePhysicalDevices() {
    GM_ASSERT(mInstance != nullptr)

    uint32_t num = 0;

    VK(vkEnumeratePhysicalDevices(mInstance, &num, nullptr));
    VkPhysicalDevice* devices = new VkPhysicalDevice[num];
    VK(vkEnumeratePhysicalDevices(mInstance, &num, devices));

    for (uint32_t i = 0; i < num; i++) {
        mPhysicalDevices.push_back(new PhysicalDevice(devices[i]));
    }

    delete devices;
}

}