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

#include "../core/uuid.h"
#include "../vulkan/commandbuffer.h"

#include <unordered_map>
#include <thread>

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
    static void AddAsset(Asset* asset, bool asyncLoad);

    template<typename T>
    static T* GetAsset(const std::filesystem::path& path) { return (T*)GetAssetInternal(path); }
    static bool IsAssetLoaded(const std::filesystem::path& path);

    static std::vector<FinishedAsset> GetFinishedAssets();
private:
    static Asset* GetAssetInternal(const std::filesystem::path& path);
    
    static void QueueWorker();
    static void LoadAssetFunction(Asset* asset);

private:
    static bool mShouldStop;
    static std::thread mLoaderThread;
    static std::mutex mQueueMutex;
    static std::mutex mCommandBufferMutex;
    static std::unordered_map<std::filesystem::path, Asset*> mAssets;
    static std::vector<FinishedAsset> mFinishedCommandBuffers;
    static std::vector<Asset*> mAssetQueue;
};

}