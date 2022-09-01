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

class CommandBuffer {
public:
    CommandBuffer(VkCommandBuffer Handle);
    ~CommandBuffer();

    void Reset() const;
    void Begin(bool oneTimeSubmit) const;
    void End() const;
    void WaitForFence() const;

    inline VkCommandBuffer GetHandle() const { return mCommandBufferHandle; }
    inline VkFence GetFence() const { return mFenceHandle; }
    inline bool IsUsed() const { return mUsed; }
private:
    VkCommandBuffer mCommandBufferHandle;
    VkFence mFenceHandle;
    mutable bool mUsed;
};

class CommandPool {
public:
    CommandPool();
    ~CommandPool();

    void Reset() const;
    std::vector<CommandBuffer*> AllocateCommandBuffers(uint32_t num, bool primary) const;

    inline VkCommandPool GetHandle() const { return mCommandPoolHandle; }
private:
    VkCommandPool mCommandPoolHandle;
};




}