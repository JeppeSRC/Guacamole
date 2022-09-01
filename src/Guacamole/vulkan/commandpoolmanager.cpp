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

std::vector<CommandPoolManager::Pool> CommandPoolManager::mRenderCommandPools;
std::vector<CommandPoolManager::Pool> CommandPoolManager::mAuxCommandPools;

void CommandPoolManager::Init(uint32_t numThreads) {

    for (uint32_t i = 0; i < numThreads; i++) {
        Pool pool;
        pool.mPool = new CommandPool;

        mRenderCommandPools.push_back(pool);

        pool.mPool = new CommandPool;

        mAuxCommandPools.push_back(pool);
    }
}

void CommandPoolManager::AllocatePrimaryRenderCommandBuffers(uint32_t imageCount) {
    for (Pool& pool : mRenderCommandPools) {
        pool.mCommandBuffers = pool.mPool->AllocateCommandBuffers(imageCount, true);
    }
}

void CommandPoolManager::AllocateAuxCommandBuffers(uint32_t count) {
    for (Pool& pool : mAuxCommandPools) {
        pool.mCommandBuffers = pool.mPool->AllocateCommandBuffers(count, true);
    }
}

CommandBuffer* CommandPoolManager::GetPrimaryRenderCommandBuffer() {
    uint32_t imageIndex = Swapchain::GetCurrentImageIndex();
    GM_ASSERT(imageIndex!= ~0);

    // TODO: retreive thread index
    uint32_t threadIndex = 0;

    return mRenderCommandPools[threadIndex].mCommandBuffers[imageIndex];
}

CommandBuffer* CommandPoolManager::GetAuxCommandBuffer(uint32_t index) {
    // TODO: retreive thread index
    uint32_t threadIndex = 0;

    return mAuxCommandPools[threadIndex].mCommandBuffers[index];
}

void CommandPoolManager::WaitForRenderFences() {
    for (Pool& pool : mRenderCommandPools) {
        for (CommandBuffer* buf : pool.mCommandBuffers) {
            buf->WaitForFence();
        }
    }
}

void CommandPoolManager::Shutdown() {
    for (Pool& pool : mRenderCommandPools) {
        delete pool.mPool;

        for (CommandBuffer* buf : pool.mCommandBuffers) {
            delete buf;
        }
    }

    for (Pool& pool : mAuxCommandPools) {
        delete pool.mPool;

        for (CommandBuffer* buf : pool.mCommandBuffers) {
            delete buf;
        }
    }
}

}
