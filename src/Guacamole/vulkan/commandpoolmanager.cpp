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
#include "swapchain.h"

namespace Guacamole {

std::unordered_map<std::thread::id, CommandPoolManager::Pool> CommandPoolManager::mRenderCommandPools;
std::unordered_map<std::thread::id, CommandPoolManager::Pool> CommandPoolManager::mCopyCommandPools;

void CommandPoolManager::AllocatePrimaryRenderCommandBuffers(std::thread::id threadId, uint32_t imageCount) {
    auto it = mRenderCommandPools.find(threadId);

    GM_VERIFY_MSG(it == mRenderCommandPools.end(), "You may only allocate buffers once per thread");

    CommandPool* pool = new CommandPool;

    mRenderCommandPools[threadId] = { pool, pool->AllocateCommandBuffers(imageCount, true) };
}

void CommandPoolManager::AllocateCopyCommandBuffers(std::thread::id threadId, uint32_t count) {
    auto it = mCopyCommandPools.find(threadId);

    GM_VERIFY_MSG(it == mCopyCommandPools.end(), "You may only allocate buffers once per thread");

    CommandPool* pool = new CommandPool;

    mCopyCommandPools[threadId] = { pool, pool->AllocateCommandBuffers(count, true) };
}

void CommandPoolManager::AllocateAssetCommandBuffers(std::thread::id threadId) {
    AllocateCopyCommandBuffers(threadId, 1);
}

CommandBuffer* CommandPoolManager::GetPrimaryRenderCommandBuffer() {
    uint32_t imageIndex = Swapchain::GetCurrentImageIndex();
    GM_ASSERT(imageIndex!= ~0);

    std::thread::id threadId = std::this_thread::get_id();

    return mRenderCommandPools[threadId].mCommandBuffers[imageIndex];
}

CommandBuffer* CommandPoolManager::GetCopyCommandBuffer(uint32_t index) {
    std::thread::id threadId = std::this_thread::get_id();

    return mCopyCommandPools[threadId].mCommandBuffers[index];
}

void CommandPoolManager::WaitForRenderFences() {
    for (auto [id, pool] : mRenderCommandPools) {
        for (CommandBuffer* buf : pool.mCommandBuffers) {
            buf->WaitForFence();
        }
    }
}

void CommandPoolManager::Shutdown() {
    for (auto [id, pool] : mRenderCommandPools) {
        delete pool.mPool;

        for (CommandBuffer* buf : pool.mCommandBuffers) {
            delete buf;
        }
    }

    for (auto [id, pool] : mCopyCommandPools) {
        delete pool.mPool;

        for (CommandBuffer* buf : pool.mCommandBuffers) {
            delete buf;
        }
    }
}

}
