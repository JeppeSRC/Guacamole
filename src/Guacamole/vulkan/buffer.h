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

class Buffer {
protected:
    VkBuffer mBufferHandle;
    VkDeviceMemory mBufferMemory;
    VkBuffer mMappedBufferHandle;
    VkDeviceMemory mMappedBufferMemory;

    uint64_t mBufferSize;
    void* mMappedMemory;

public:
    Buffer(VkBufferUsageFlags usage, uint64_t size, void* data = nullptr);
    virtual ~Buffer();

    void* Map();
    void Unmap();
    void WriteData(void* data, uint64_t size, uint64_t offset = 0);
    void WriteDataImmediate(void* data, uint64_t size, uint64_t offset = 0);
    void StageCopy(bool immediate);

    inline VkBuffer GetHandle() const { return mBufferHandle; }
    inline uint64_t GetSize() const { return mBufferSize; }

public:
    static uint32_t GetMemoryIndex(const VkPhysicalDeviceMemoryProperties props, uint32_t type, VkMemoryPropertyFlags flags);
};



}