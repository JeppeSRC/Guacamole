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

#include <fstream>

#include "util.h"


#define MAKE_CASE(name) case name: return #name

namespace Guacamole { namespace Util {

const char* vkEnumToString(VkPhysicalDeviceType type) {
    switch (type) {
        MAKE_CASE(VK_PHYSICAL_DEVICE_TYPE_OTHER);
        MAKE_CASE(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
        MAKE_CASE(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        MAKE_CASE(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
        MAKE_CASE(VK_PHYSICAL_DEVICE_TYPE_CPU);
    }

    return "(null)";
}


uint8_t* ReadFile(const std::filesystem::path& file, uint64_t& fileSize) {
    if (!std::filesystem::exists(file)) {
        GM_LOG_CRITICAL("File \"{0}\" doesn't exist!", file.string().c_str());
        return nullptr;
    }

    fileSize = std::filesystem::file_size(file);

    if (fileSize == 0) {
        return nullptr;
    }

    uint8_t* data = new uint8_t[fileSize];

    FILE* f = fopen(file.string().c_str(), "rb");

    if (f == nullptr) {
        GM_LOG_CRITICAL("Failed to open file \"{0}\"", file.string().c_str());
        return nullptr;
    }

    fread(data, fileSize, 1, f);
    fclose(f);

    return data;
}

VkFormat SPIRTypeToVkFormat(spirv_cross::SPIRType type) {

    using namespace spirv_cross;

    if (type.basetype == SPIRType::Float) {
        GM_VERIFY(type.width == 32);

        if (type.columns == 1) {
            switch (type.vecsize) {
                case 1:
                    return VK_FORMAT_R32_SFLOAT;
                case 2:
                    return VK_FORMAT_R32G32_SFLOAT;
                case 3:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case 4:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
        }
    } else if (type.basetype == SPIRType::Int) {
        GM_VERIFY(type.width == 32);

        if (type.columns == 1) {
            switch (type.vecsize) {
                case 1:
                    return VK_FORMAT_R32_SINT;
                case 2:
                    return VK_FORMAT_R32G32_SINT;
                case 3:
                    return VK_FORMAT_R32G32B32_SINT;
                case 4:
                    return VK_FORMAT_R32G32B32A32_SINT;
            }
        }
    } else if (type.basetype == SPIRType::UInt) {
        GM_VERIFY(type.width == 32);

        if (type.columns == 1) {
            switch (type.vecsize) {
                case 1:
                    return VK_FORMAT_R32_UINT;
                case 2:
                    return VK_FORMAT_R32G32_UINT;
                case 3:
                    return VK_FORMAT_R32G32B32_UINT;
                case 4:
                    return VK_FORMAT_R32G32B32A32_UINT;
            }
        }
    }

    GM_VERIFY(false);

    return VK_FORMAT_UNDEFINED;
}

}
}