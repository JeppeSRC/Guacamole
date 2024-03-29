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

class Device;
class Buffer {
protected:
    VkBuffer mBufferHandle;
    VkDeviceMemory mBufferMemory;
    VkMemoryPropertyFlags mBufferFlags;

    uint64_t mBufferSize;
    void* mMappedMemory;

    Device* mDevice;
    
    void Create(Device* device, VkBufferUsageFlags usage, uint64_t size, VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

public:
    Buffer(VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    Buffer(Device* device, VkBufferUsageFlags usage, uint64_t size, VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    Buffer(Buffer&& other);
    Buffer(const Buffer& other) = delete; // No copies allowed
    virtual ~Buffer();

    void* Map();
    void Unmap();

    Buffer& operator=(const Buffer&) = delete;

    Buffer& operator=(Buffer&& other) {
        mBufferHandle = other.mBufferHandle;
        mBufferMemory = other.mBufferMemory;
        mBufferFlags = other.mBufferFlags;
        mBufferSize = other.mBufferSize;
        mMappedMemory = other.mMappedMemory;

        other.mBufferMemory = VK_NULL_HANDLE;
        other.mBufferHandle = VK_NULL_HANDLE;

        return *this;
    }

    inline const VkBuffer& GetHandle() const { return mBufferHandle; }
    inline uint64_t GetSize() const { return mBufferSize; }

public:
    static uint32_t GetMemoryIndex(const VkPhysicalDeviceMemoryProperties props, uint32_t type, VkMemoryPropertyFlags flags);
};

class UniformBuffer : public Buffer {
public:
    UniformBuffer() : Buffer(), mBinding(0) {}
    UniformBuffer(Device* device, uint64_t size, uint32_t binding) : Buffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size), mBinding(binding) {}

private:
    uint32_t mBinding;
};

class IndexBuffer : public Buffer {
public:
    IndexBuffer(Device* device, uint32_t count, VkIndexType type);

    inline uint32_t GetCount() const { return mCount; }
    inline VkIndexType GetIndexType() const { return mIndexType; }
private:
    uint32_t mCount;
    VkIndexType mIndexType;
};

class VertexBuffer : public Buffer {
public:
    VertexBuffer(Device* device, uint64_t size) : Buffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size) {}
};

}