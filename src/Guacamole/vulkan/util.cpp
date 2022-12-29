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

#include "util.h"
#include "device.h"

namespace Guacamole {

uint64_t GetFormatSize(VkFormat format) {
    switch (format) {
        // 8 bit formats
        case VK_FORMAT_R8_SINT:
        case VK_FORMAT_R8_SNORM:
        case VK_FORMAT_R8_SRGB:
        case VK_FORMAT_R8_SSCALED:
        case VK_FORMAT_R8_UINT:
        case VK_FORMAT_R8_UNORM:
        case VK_FORMAT_R8_USCALED:
            return 1;
        case VK_FORMAT_R8G8_SINT:
        case VK_FORMAT_R8G8_SNORM:
        case VK_FORMAT_R8G8_SRGB:
        case VK_FORMAT_R8G8_SSCALED:
        case VK_FORMAT_R8G8_UINT:
        case VK_FORMAT_R8G8_UNORM:
        case VK_FORMAT_R8G8_USCALED:
            return 2;
        case VK_FORMAT_R8G8B8_SINT:
        case VK_FORMAT_R8G8B8_SNORM:
        case VK_FORMAT_R8G8B8_SRGB:
        case VK_FORMAT_R8G8B8_SSCALED:
        case VK_FORMAT_R8G8B8_UINT:
        case VK_FORMAT_R8G8B8_UNORM:
        case VK_FORMAT_R8G8B8_USCALED:
            return 3;
        case VK_FORMAT_R8G8B8A8_SINT:
        case VK_FORMAT_R8G8B8A8_SNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
        case VK_FORMAT_R8G8B8A8_SSCALED:
        case VK_FORMAT_R8G8B8A8_UINT:
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_USCALED:
            return 4;

        // 16 bit formats
        case VK_FORMAT_R16_SINT:
        case VK_FORMAT_R16_SNORM:
        case VK_FORMAT_R16_SFLOAT:
        case VK_FORMAT_R16_SSCALED:
        case VK_FORMAT_R16_UINT:
        case VK_FORMAT_R16_UNORM:
        case VK_FORMAT_R16_USCALED:
            return 2;
        case VK_FORMAT_R16G16_SINT:
        case VK_FORMAT_R16G16_SNORM:
        case VK_FORMAT_R16G16_SFLOAT:
        case VK_FORMAT_R16G16_SSCALED:
        case VK_FORMAT_R16G16_UINT:
        case VK_FORMAT_R16G16_UNORM:
        case VK_FORMAT_R16G16_USCALED:
            return 4;
        case VK_FORMAT_R16G16B16_SINT:
        case VK_FORMAT_R16G16B16_SNORM:
        case VK_FORMAT_R16G16B16_SFLOAT:
        case VK_FORMAT_R16G16B16_SSCALED:
        case VK_FORMAT_R16G16B16_UINT:
        case VK_FORMAT_R16G16B16_UNORM:
        case VK_FORMAT_R16G16B16_USCALED:
            return 6;
        case VK_FORMAT_R16G16B16A16_SINT:
        case VK_FORMAT_R16G16B16A16_SNORM:
        case VK_FORMAT_R16G16B16A16_SFLOAT:
        case VK_FORMAT_R16G16B16A16_SSCALED:
        case VK_FORMAT_R16G16B16A16_UINT:
        case VK_FORMAT_R16G16B16A16_UNORM:
        case VK_FORMAT_R16G16B16A16_USCALED:
            return 8;

        // 32 bit formats
        case VK_FORMAT_R32_SINT:
        case VK_FORMAT_R32_SFLOAT:
        case VK_FORMAT_R32_UINT:
            return 4;
        case VK_FORMAT_R32G32_SINT:
        case VK_FORMAT_R32G32_UINT:
        case VK_FORMAT_R32G32_SFLOAT:
            return 8;
        case VK_FORMAT_R32G32B32_SINT:
        case VK_FORMAT_R32G32B32_UINT:
        case VK_FORMAT_R32G32B32_SFLOAT:
            return 12;
        case VK_FORMAT_R32G32B32A32_SINT:
        case VK_FORMAT_R32G32B32A32_UINT:
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return 16;
        default:
            GM_ASSERT_MSG(false, "Format not implemented");
    }

    return 0;
}

#define YEET(res) case res: return #res

const char* GetVkResultString(VkResult result) {
    switch (result) {
        YEET(VK_SUCCESS);
        YEET(VK_NOT_READY);
        YEET(VK_TIMEOUT);
        YEET(VK_EVENT_SET);
        YEET(VK_EVENT_RESET);
        YEET(VK_INCOMPLETE);
        YEET(VK_ERROR_OUT_OF_HOST_MEMORY);
        YEET(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        YEET(VK_ERROR_INITIALIZATION_FAILED);
        YEET(VK_ERROR_DEVICE_LOST);
        YEET(VK_ERROR_MEMORY_MAP_FAILED);
        YEET(VK_ERROR_LAYER_NOT_PRESENT);
        YEET(VK_ERROR_EXTENSION_NOT_PRESENT);
        YEET(VK_ERROR_FEATURE_NOT_PRESENT);
        YEET(VK_ERROR_INCOMPATIBLE_DRIVER);
        YEET(VK_ERROR_TOO_MANY_OBJECTS);
        YEET(VK_ERROR_FORMAT_NOT_SUPPORTED);
        YEET(VK_ERROR_FRAGMENTED_POOL);
        YEET(VK_ERROR_UNKNOWN);
        YEET(VK_ERROR_OUT_OF_POOL_MEMORY);
        YEET(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        YEET(VK_ERROR_FRAGMENTATION);
        YEET(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
        YEET(VK_PIPELINE_COMPILE_REQUIRED);
        YEET(VK_ERROR_SURFACE_LOST_KHR);
        YEET(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        YEET(VK_SUBOPTIMAL_KHR);
        YEET(VK_ERROR_OUT_OF_DATE_KHR);
        YEET(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        YEET(VK_ERROR_VALIDATION_FAILED_EXT);
        YEET(VK_ERROR_INVALID_SHADER_NV);
        YEET(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        YEET(VK_ERROR_NOT_PERMITTED_KHR);
        YEET(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        YEET(VK_THREAD_IDLE_KHR);
        YEET(VK_THREAD_DONE_KHR);
        YEET(VK_OPERATION_DEFERRED_KHR);
        YEET(VK_OPERATION_NOT_DEFERRED_KHR);
        YEET(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
        default:
            GM_ASSERT_MSG(false, "Not implemented");
    }

    return "unkown";
}

CircularSemaphorePool::CircularSemaphorePool(Device* device, uint32_t count) : CircularPool<VkSemaphore>(device, count) {
        Init(count);
}

CircularSemaphorePool::~CircularSemaphorePool() {
    for (uint32_t i = 0; i < mCount; i++) {
        vkDestroySemaphore(mDevice->GetHandle(), mResource[i], nullptr);
    }

    delete[] mResource;
}

void CircularSemaphorePool::Init(uint32_t count) {
    if (count == 0) return;

    mCount = count;

    mResource = new VkSemaphore[mCount];

    VkSemaphoreCreateInfo info;

    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    for (uint32_t i = 0; i < count; i++) {
        VK(vkCreateSemaphore(mDevice->GetHandle(), &info, nullptr, &mResource[i]));
    }
}

}