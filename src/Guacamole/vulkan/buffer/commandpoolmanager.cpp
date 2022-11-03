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

#include "commandpoolmanager.h"
#include <Guacamole/vulkan/swapchain.h>

namespace Guacamole {

CommandPoolManager::Pool CommandPoolManager::mRenderCommandPool;
std::unordered_map<std::thread::id, CommandPoolManager::Pool> CommandPoolManager::mAuxCommandPools;

void CommandPoolManager::AllocatePrimaryRenderCommandBuffers(uint32_t imageCount) {
    GM_ASSERT_MSG(mRenderCommandPool.mPool == nullptr, "Primary render command buffers already allocated");
    mRenderCommandPool.mPool = new CommandPool;
    mRenderCommandPool.mCommandBuffers = mRenderCommandPool.mPool->AllocateCommandBuffers(imageCount, true);
}

CommandBuffer* CommandPoolManager::AllocateAuxCommandBuffer(std::thread::id threadId, bool primary) {
    auto it = mAuxCommandPools.find(threadId);

    if (it != mAuxCommandPools.end()) {
        Pool& pool = it->second;
        CommandBuffer* buffer = pool.mPool->AllocateCommandBuffers(1, primary)[0];

        pool.mCommandBuffers.push_back(buffer);

        return buffer;
    }

    CommandPool* pool = new CommandPool;
    std::vector<CommandBuffer*> buffers = pool->AllocateCommandBuffers(1, primary);

    mAuxCommandPools[threadId] = { pool, buffers };

    return buffers[0];
}

CommandBuffer* CommandPoolManager::GetRenderCommandBuffer() {
    uint32_t imageIndex = Swapchain::GetCurrentImageIndex();
    GM_ASSERT(imageIndex!= ~0);

    return mRenderCommandPool.mCommandBuffers[imageIndex];
}

CommandBuffer* CommandPoolManager::GetCopyCommandBuffer(uint32_t index) {
    std::thread::id threadId = std::this_thread::get_id();

    return mAuxCommandPools[threadId].mCommandBuffers[index];
}

void CommandPoolManager::WaitForRenderFences() {
    for (CommandBuffer* buf : mRenderCommandPool.mCommandBuffers) {
        buf->WaitForFence();
    }
}

void CommandPoolManager::Shutdown() {
    delete mRenderCommandPool.mPool;

    for (CommandBuffer* buf : mRenderCommandPool.mCommandBuffers) {
        delete buf;
    }

    for (auto [id, pool] : mAuxCommandPools) {
        delete pool.mPool;

        for (CommandBuffer* buf : pool.mCommandBuffers) {
            delete buf;
        }
    }
}

}
