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
    static void Init(Device* device);
    static void Shutdown();

    static AssetHandle AddNewAsset(const std::string& filename);

    template<typename T = Asset>
    static T* GetAsset(AssetHandle handle) { return (T*)GetAssetInternal(handle); }
    static bool IsAssetLoaded(AssetHandle handle);
    static bool IsAssetDataLoaded(AssetHandle handle);
private:
    static Asset* GetAssetInternal(AssetHandle handle);
    static AssetData* GetAssetDataInternal(AssetHandle handle);
    
    static void QueueWorker();
    static VkPipelineStageFlags LoadAsset(AssetHandle handle);
    static VkPipelineStageFlags LoadMeshAsset(AssetHandle handle, AssetData* assetData, File* file);
    static VkPipelineStageFlags LoadTextureAsset(AssetHandle handle, AssetData* assetData, File* file);
    static VkPipelineStageFlags LoadShaderAsset(AssetHandle handle, AssetData* assetData, File* file);

private:
    static Device* mDevice;
    static bool mShouldStop;
    static std::thread mLoaderThread;
    static std::mutex mQueueMutex;
    static std::mutex mAssetMutex;
    static std::mutex mAssetDataMutex;
    static std::unordered_map<AssetHandle, Asset*> mAssets;
    static std::unordered_map<AssetHandle, Asset*> mMemoryAssets;
    static std::unordered_map<AssetHandle, AssetData*> mAssetData;
    static std::vector<AssetHandle> mAssetQueue;
};

}