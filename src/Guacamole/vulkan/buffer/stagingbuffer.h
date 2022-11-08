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

#include "buffer.h"
#include "commandbuffer.h"
#include <Guacamole/vulkan/shader/texture.h>

#include <thread>

namespace Guacamole {

class StagingBuffer {
public:
    StagingBuffer(uint64_t mSize, CommandBuffer* commandBuffer = nullptr);
    ~StagingBuffer();

    void Begin(bool waitFence = true);
    void* Allocate(uint64_t size, Buffer* buffer, uint64_t bufferOffset = 0);
    void* AllocateImage(VkImageLayout oldLayout, VkImageLayout newLayout, Texture* texture, uint32_t mip = 0);
    void Reset();

    inline uint64_t GetAllocated() const { return mAllocated; }
    inline const Buffer* GetBuffer() const { return &mBuffer; }
    inline uint64_t GetSize() const { return mBuffer.GetSize(); }
    inline CommandBuffer* GetCommandBuffer() const { return mCommandBuffer; }
private:
    uint64_t mAllocated;
    Buffer mBuffer;
    uint8_t* mMemory;
    CommandBuffer* mCommandBuffer;

public:
    // This is called once pre thread
    static void AllocateStagingBuffer(std::thread::id id, uint64_t size);
    static void Shutdown();
    static StagingBuffer* GetStagingBuffer() { return mStagingBuffers[std::this_thread::get_id()]; }

private:
    static std::unordered_map<std::thread::id, StagingBuffer*> mStagingBuffers;

};

}