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
VkDebugUtilsMessengerEXT Context::mMessenger;

VkBool32 VKAPI_PTR VkDebugMessagerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
    if (callbackData->pMessage == nullptr) return false;

    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            GM_LOG_CRITICAL("[ValidationLayer] {}", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            GM_LOG_WARNING("[ValidationLayer] {}", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            GM_LOG_INFO("[ValidationLayer] {}", callbackData->pMessage);
            break;
        default:
            GM_LOG_INFO("[ValidationLayer_] {}", callbackData->pMessage);
    }

    return false;
}

PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger;
PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger;

void Context::Init(const ContextSpec& spec) {
    ScopedTimer timer1("Context::Init");
    uint32_t version = 0;

    vkEnumerateInstanceVersion(&version);

    uint32_t minVersion = GM_VK_MIN_VERSION;

    uint32_t major = VK_API_VERSION_MAJOR(version);
    uint32_t minor = VK_API_VERSION_MINOR(version);

    GM_LOG_DEBUG("Max Instance Version: {}.{}", major, minor);

    GM_VERIFY(version >= minVersion);
    
    EnumerateLayersAndExtensions();

    VkApplicationInfo appInfo;

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.apiVersion = minVersion;
    appInfo.pApplicationName = spec.applicationName.c_str();
    appInfo.engineVersion = 0;
    appInfo.pEngineName = "Guacamole";

    VkInstanceCreateInfo instanceInfo;

    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.flags = 0;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;

    std::vector<const char*> extensions {"VK_KHR_surface"};

#if defined(GM_LINUX)
    extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(GM_WINDOWS)
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif


    bool dbgEnabled = false;

    #if 1

    const char* layer = "VK_LAYER_KHRONOS_validation";
    const char* dbgExtension = "VK_EXT_debug_utils";

    if (IsLayerSupported(layer)) {
        instanceInfo.enabledLayerCount = 1;
        instanceInfo.ppEnabledLayerNames = &layer;

        extensions.push_back(dbgExtension);
        dbgEnabled = true;
    } else {
        GM_LOG_WARNING("VK_LAYER_KHRONOS_validation not supported");
    }
#endif
    instanceInfo.enabledExtensionCount = extensions.size();
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    VK(vkCreateInstance(&instanceInfo, nullptr, &mInstance));

    vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT");

    if (dbgEnabled) {
        VkDebugUtilsMessengerCreateInfoEXT dbgCreate;

        dbgCreate.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        dbgCreate.pNext = nullptr;
        dbgCreate.flags = 0;
        dbgCreate.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        dbgCreate.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        dbgCreate.pfnUserCallback = VkDebugMessagerCallback;
        dbgCreate.pUserData = nullptr;
        
        VK(vkCreateDebugUtilsMessenger(mInstance, &dbgCreate, nullptr, &mMessenger));
    }

    EnumeratePhysicalDevices();
}


void Context::Shutdown() {
    for (Device* device : mDevices) {
        GM_LOG_DEBUG("[Context] Destroyed device \"{}\"", device->GetParent()->GetProperties().deviceName);
        delete device;
    }

    mDevices.clear();

    vkDestroyDebugUtilsMessenger(mInstance, mMessenger, nullptr);
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

    delete[] props;
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

    delete[] devices;
}

}