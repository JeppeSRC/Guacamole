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
#include <Guacamole/util/util.h>
#include "shader.h"
#include "context.h"

namespace Guacamole {

Shader::Shader(const std::string& file, bool src) : ModuleHandle(VK_NULL_HANDLE) {
    uint64_t size;
    void* data = Util::ReadFile(file, size);

    if (data == nullptr) {
        GM_LOG_CRITICAL("Failed to load shader file \"{0}\"", file.c_str());
        return;
    }

    VkShaderModuleCreateInfo mInfo;

    mInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    mInfo.pNext = nullptr;
    mInfo.flags = 0;

    if (src) {
        //TODO
    } else {
        if (size & 0x03) {
            GM_LOG_WARNING("Shader binary \"{0}\" size is not a multiple of 4", file.c_str());

            uint64_t newSize = (size & ~0x03) + 4;
            uint8_t* newData = new uint8_t[newSize];

            memcpy(newData, data, size);

            delete[] data;

            data = newData;
            size = newSize;
        }

        mInfo.codeSize = size;
        mInfo.pCode = (uint32_t*)data;
    }

    VK(vkCreateShaderModule(Context::GetDeviceHandle(), &mInfo, nullptr, &ModuleHandle));

    delete[] data;
}

Shader::~Shader() {
    vkDestroyShaderModule(Context::GetDeviceHandle(), ModuleHandle, nullptr);
}

}