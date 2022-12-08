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

#include <Guacamole/vulkan/semaphore.h>

namespace Guacamole {

class Device;
class CommandBuffer {
public:
    CommandBuffer(Device* device, VkCommandBuffer Handle);
    ~CommandBuffer();

    void Reset() const;
    void Begin(bool oneTimeSubmit) const;
    void End() const;
    void Wait() const;

    inline const VkCommandBuffer& GetHandle() const { return mCommandBufferHandle; }
    inline Semaphore* GetSemaphore() const { return mSemaphore; }
    inline bool IsUsed() const { return mUsed; }
private:
    VkCommandBuffer mCommandBufferHandle;
    mutable bool mUsed;
    Semaphore* mSemaphore;

    Device* mDevice;
};

class CommandPool {
public:
    CommandPool(Device* device);
    ~CommandPool();

    void Reset() const;
    std::vector<CommandBuffer*> AllocateCommandBuffers(uint32_t num, bool primary) const;
    CommandBuffer* AllocateCommandBuffer(bool primary) const;

    inline const VkCommandPool& GetHandle() const { return mCommandPoolHandle; }
private:
    VkCommandPool mCommandPoolHandle;
    Device* mDevice;
};




}