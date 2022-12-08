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

#include "assetmanager.h"

#include <Guacamole/vulkan/buffer/commandbuffer.h>
#include <Guacamole/vulkan/buffer/stagingbuffer.h>
#include <Guacamole/vulkan/device.h>

#if defined(GM_LINUX)

namespace std {
    template<>
    struct hash<std::filesystem::path> {
        size_t operator()(const std::filesystem::path& path) const {
            return hash_value(path);
        }
    };
}

#endif

namespace Guacamole {

Device* AssetManager::mDevice;
bool AssetManager::mShouldStop;
std::thread AssetManager::mLoaderThread;
std::mutex AssetManager::mQueueMutex;
std::mutex AssetManager::mCommandBufferMutex;
std::unordered_map<AssetHandle, Asset*> AssetManager::mAssets;
std::vector<Asset*> AssetManager::mAssetQueue;



void AssetManager::Init(Device* device) {
    mShouldStop = false;
    mDevice = device;
    mLoaderThread = std::thread(&AssetManager::QueueWorker);
}

void AssetManager::Shutdown() {
    mShouldStop = true;
    mLoaderThread.join();

    for (auto [handle, asset] : mAssets) {
        if (asset->mFlags & AssetFlag_MemoryAsset) {
            if (asset->mFlags & AssetFlag_OwnsMemory) delete asset;
        } else {
            delete asset;
        }
    }
}

AssetHandle AssetManager::AddAsset(Asset* asset, bool asyncLoad) {
    AssetHandle handle = asset->mHandle;

    if (asset->mFilePath.empty()) {
        GM_LOG_CRITICAL("Asset \"AssetHandle: 0x{:08x}\" has no path!", handle);
        return AssetHandle::Null();
    }

    AssetHandle tmp = GetAssetHandleFromPath(asset->mFilePath);

    if (tmp != AssetHandle::Null()) {
        GM_LOG_CRITICAL("Asset Path: \"{}\" already exist!", asset->GetPathAsString().c_str());
        return tmp;
    }

    mAssets[handle] = asset;
    
    GM_LOG_DEBUG("Added asset Path: \"{}\" AssetHandle: 0x{:08x}", asset->GetPathAsString().c_str(), handle);

    if (asyncLoad) {
        mQueueMutex.lock();
        asset->mFlags |= AssetFlag_Loading;
        mAssetQueue.push_back(asset);
        mQueueMutex.unlock();
        GM_LOG_DEBUG("Asset Path: \"{}\" AssetHandle: 0x{:08x} Added To Queue!", asset->GetPathAsString().c_str(), handle);
    } else {
        asset->Load();
        GM_LOG_DEBUG("Asset Path: \"{}\" AssetHandle: 0x{:08x} Loaded!", asset->GetPathAsString().c_str(), handle);
    }

    return handle;
}

AssetHandle AssetManager::AddMemoryAsset(Asset* asset, bool takeOwnershipOfMemory) {
    AssetHandle handle = asset->mHandle;

    if (mAssets.find(handle) != mAssets.end()) {
        GM_LOG_CRITICAL("Asset \"AssetHandle: 0x{:08x}\" already exist!", handle);
        return AssetHandle::Null();
    }

    mAssets[handle] = asset;

    asset->mFlags |= AssetFlag_MemoryAsset;

    if (takeOwnershipOfMemory) {
        asset->mFlags |= AssetFlag_OwnsMemory;
    }

    GM_LOG_DEBUG("Added Asset \"AssetHandle: {:08x}\"", handle);

    return handle;
}

Asset* AssetManager::GetAssetInternal(AssetHandle handle) {
    const auto& itr = mAssets.find(handle);

    if (itr == mAssets.end()) {
        GM_LOG_CRITICAL("Asset [handle: {:08x}] doesn't exist", handle);
        return nullptr;
    }

    Asset* asset = itr->second;

    while (!asset->IsLoaded()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return asset;
}

bool AssetManager::IsAssetLoaded(AssetHandle handle) {
    return mAssets.at(handle)->IsLoaded();
}

AssetHandle AssetManager::GetAssetHandleFromPath(const std::filesystem::path& path) {
    for (auto [handle, asset] : mAssets) {
        if (asset->mFilePath == path) return handle;
    }

    return AssetHandle::Null();
}

void AssetManager::QueueWorker() {
    StagingManager::AllocateCommonStagingBuffer(mDevice, std::this_thread::get_id(), 24000000, true); // 24MB

    while (!mShouldStop) {
        mQueueMutex.lock();

        if (mAssetQueue.size() == 0) {
            mQueueMutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        StagingBuffer* buf = StagingManager::GetCommonStagingBuffer();

        buf->Begin();

        Asset* currentAsset = mAssetQueue.front();

        mAssetQueue.erase(mAssetQueue.begin());
        mQueueMutex.unlock();

        bool usedBuffer = LoadAssetFunction(currentAsset);
        currentAsset->mFlags &= ~AssetFlag_Loading;

        if (usedBuffer) {
            StagingManager::SubmitStagingBuffer(buf, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
        } else {
            buf->GetCommandBuffer()->End();
        }
    }
}

bool AssetManager::LoadAssetFunction(Asset* asset) {
    AssetHandle handle = asset->mHandle;

    GM_LOG_DEBUG("Loading asset Path: \"{}\" AssetHandle: 0x{:08x}", asset->GetPathAsString().c_str(), handle);

    bool usedBuffer = asset->Load();

    if (asset->IsLoaded()) {
        GM_LOG_DEBUG("Asset Path: \"{}\" AssetHandle: 0x{:08x} Loaded!", asset->GetPathAsString().c_str(), handle);
    } else {
        GM_LOG_DEBUG("Asset Path: \"{}\" AssetHandle: 0x{:08x} Not loaded!", asset->GetPathAsString().c_str(), handle);
    }

    return usedBuffer;
}

}