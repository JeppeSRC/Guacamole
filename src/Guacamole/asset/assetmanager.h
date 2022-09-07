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

#include <unordered_map>
#include <thread>

namespace std {

template<>
struct hash<std::filesystem::path> {

    std::size_t operator()(const std::filesystem::path& path) const {
        return hash_value(path);
    }
};

}

namespace Guacamole {

class AssetManager {
public:
    typedef std::function<void(Asset*, void*)> AssetLoadedCallback;
public:
    static void Init();
    static void Shutdown();
    static void AddAsset(Asset* asset, bool load);

    static Asset* GetAsset(const std::filesystem::path& path);
    static Asset* GetAssetAsync(const std::filesystem::path& path, AssetLoadedCallback callback, void* userData);

    static void ProcessCallbacks();
private:

    struct LoadAssetData {
        Asset* mAsset;
        std::vector<std::pair<AssetLoadedCallback, void*>> mCallbacks;
    };

    static void QueueWorker();
    static void LoadAssetFunction(LoadAssetData* data);
    static void AddAssetToQueue(Asset* asset, AssetLoadedCallback callback, void* userData);
private:
    static bool mShouldStop;
    static std::thread mLoaderThread;
    static std::mutex mQueueMutex;
    static std::mutex mCallbackMutex;
    static std::mutex mCurrentAssetMutex;
    static LoadAssetData* mCurrentAsset;
    static std::unordered_map<std::filesystem::path, Asset*> mAssets;
    static std::vector<LoadAssetData*> mAssetCallbacks;
    static std::vector<LoadAssetData*> mAssetQueue;
};

}