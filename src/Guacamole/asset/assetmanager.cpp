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

namespace Guacamole {

bool AssetManager::mShouldStop;
std::thread AssetManager::mLoaderThread;
std::mutex AssetManager::mQueueMutex;
std::mutex AssetManager::mCallbackMutex;
std::mutex AssetManager::mCurrentAssetMutex;
AssetManager::LoadAssetData* AssetManager::mCurrentAsset = nullptr;
std::unordered_map<std::filesystem::path, Asset*> AssetManager::mAssets;
std::vector<AssetManager::LoadAssetData*> AssetManager::mAssetCallbacks;
std::vector<AssetManager::LoadAssetData*> AssetManager::mAssetQueue;



void AssetManager::Init() {
    mShouldStop = false;
    mLoaderThread = std::thread(&AssetManager::QueueWorker);
}

void AssetManager::Shutdown() {
    mShouldStop = true;
    mLoaderThread.join();
}

void AssetManager::AddAsset(Asset* asset, bool load) {
    if (mAssets.find(asset->GetPath()) != mAssets.end()) {
        GM_LOG_WARNING("Asset \"{}\" already exist!", asset->GetPathAsString().c_str());
        return;
    }

    mAssets[asset->GetPath()] = asset;

    GM_LOG_DEBUG("Added asset \"{}\" Load: {}", asset->GetPathAsString().c_str(), load);

    if (load) {
        asset->Load();

        GM_LOG_DEBUG("Asset \"{}\" Loaded!", asset->GetPathAsString().c_str());
    }

}

Asset* AssetManager::GetAsset(const std::filesystem::path& path) {
    return mAssets.at(path);
}

Asset* AssetManager::GetAssetAsync(const std::filesystem::path& path, AssetLoadedCallback callback, void* userData) {
    Asset* asset = mAssets.at(path);

    if (asset->IsLoaded()) return asset;
    
    if (!asset->mLoading) {
        asset->mLoading = true;

        AddAssetToQueue(asset, callback, userData);
    } else {
        mCurrentAssetMutex.lock();
        if (mCurrentAsset) {
            mCurrentAsset->mCallbacks.push_back({ callback, userData });
            mCurrentAssetMutex.unlock();
        } else {
            mCurrentAssetMutex.unlock();

            if (asset->IsLoaded()) {
                // Only return the loaded asset if it actually loaded
                return asset;
            } 
        }
    }

    //TODO: return default asset
    return nullptr;
}

void AssetManager::ProcessCallbacks() {
    if (mAssetCallbacks.size() == 0) return;

    mCallbackMutex.lock();

    for (LoadAssetData* data : mAssetCallbacks) {
        for (auto [fn, userData] : data->mCallbacks) {
            fn(data->mAsset, userData);
        }

        delete data;
    }

    mAssetCallbacks.clear();
    mCallbackMutex.unlock();
}

void AssetManager::QueueWorker() {
    while (!mShouldStop) {
        if (mAssetQueue.size() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        mCurrentAsset = mAssetQueue.front();

        mQueueMutex.lock();
        mAssetQueue.erase(mAssetQueue.begin());
        mQueueMutex.unlock();

        LoadAssetFunction(mCurrentAsset);

        // Make sure load function sets mLoading to false
        GM_ASSERT(mCurrentAsset->mAsset->mLoading == false);
        mCurrentAssetMutex.lock();
        mCurrentAsset = nullptr;
        mCurrentAssetMutex.unlock();
    }
}

void AssetManager::LoadAssetFunction(LoadAssetData* data) {
    GM_LOG_DEBUG("Loading asset \"{}\"", data->mAsset->GetPathAsString().c_str());

    data->mAsset->Load();

    mCallbackMutex.lock();
    mAssetCallbacks.push_back(data);
    mCallbackMutex.unlock();

    GM_LOG_DEBUG("Asset \"{}\" Loaded!", data->mAsset->GetPathAsString().c_str());
}

void AssetManager::AddAssetToQueue(Asset* asset, AssetLoadedCallback callback, void* userData) {
    LoadAssetData* data = new LoadAssetData;

    data->mAsset = asset;
    data->mCallbacks.push_back({ callback, userData });

    mQueueMutex.lock();
    mAssetQueue.push_back(data);
    mQueueMutex.unlock();
    GM_LOG_DEBUG("Asset \"{}\" Added to queue", asset->GetPathAsString().c_str());
}


}