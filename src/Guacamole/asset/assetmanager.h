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

#include "asset.h"

#include <Guacamole/core/uuid.h>
#include <Guacamole/vulkan/buffer/commandbuffer.h>

#include <unordered_map>
#include <thread>


namespace std {
template<>
struct hash<Guacamole::AssetHandle> {

    std::size_t operator()(const Guacamole::AssetHandle& handle) const {
        return handle.m0;
    }

};
}

namespace Guacamole {

class AssetManager {
public:
    struct FinishedAsset {
        Asset* mAsset;
        CommandBuffer* mCommandBuffer;
    };

public:
    static void Init();
    static void Shutdown();
    static AssetHandle AddAsset(Asset* asset, bool asyncLoad);
    static AssetHandle AddMemoryAsset(Asset* asset, bool takeOwnershipOfMemory = true);

    template<typename T = Asset>
    static T* GetAsset(AssetHandle handle) { return (T*)GetAssetInternal(handle); }
    static AssetType GetAssetType(AssetHandle handle) { return GetAssetInternal(handle)->mType; }
    static bool IsAssetLoaded(AssetHandle handle);
    static AssetHandle GetAssetHandleFromPath(const std::filesystem::path& path);

    static std::vector<FinishedAsset> GetFinishedAssets();
private:
    static Asset* GetAssetInternal(AssetHandle handle);
    
    static void QueueWorker();
    static void LoadAssetFunction(Asset* asset);

private:
    static bool mShouldStop;
    static std::thread mLoaderThread;
    static std::mutex mQueueMutex;
    static std::mutex mCommandBufferMutex;
    static std::unordered_map<AssetHandle, Asset*> mAssets;
    static std::vector<FinishedAsset> mFinishedCommandBuffers;
    static std::vector<Asset*> mAssetQueue;
};

}