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
#include <Guacamole/vulkan/commandpoolmanager.h>

namespace std {
    template<>
    struct hash<std::filesystem::path> {
        size_t operator()(const std::filesystem::path& path) const {
            return hash_value(path);
        }
    };
}

namespace Guacamole {

bool AssetManager::mShouldStop;
std::thread AssetManager::mLoaderThread;
std::mutex AssetManager::mQueueMutex;
std::mutex AssetManager::mCommandBufferMutex;
std::unordered_map<std::filesystem::path, Asset*> AssetManager::mAssets;
std::vector<AssetManager::FinishedAsset> AssetManager::mFinishedCommandBuffers;
std::vector<Asset*> AssetManager::mAssetQueue;



void AssetManager::Init() {
    mShouldStop = false;
    mLoaderThread = std::thread(&AssetManager::QueueWorker);
}

void AssetManager::Shutdown() {
    mShouldStop = true;
    mLoaderThread.join();

    for (auto [path, asset] : mAssets) {
        delete asset;
    }
}

void AssetManager::AddAsset(Asset* asset, bool asyncLoad) {
    if (mAssets.find(asset->GetPath()) != mAssets.end()) {
        GM_LOG_WARNING("Asset \"{}\" already exist!", asset->GetPathAsString().c_str());
        return;
    }

    mAssets[asset->GetPath()] = asset;

    GM_LOG_DEBUG("Added asset \"{}\" Load: {}", asset->GetPathAsString().c_str(), asyncLoad);

    if (asyncLoad) {
        mQueueMutex.lock();
        mAssetQueue.push_back(asset);
        mQueueMutex.unlock();
        GM_LOG_DEBUG("Asset \"{}\" Added To Queue!", asset->GetPathAsString().c_str());
    } else {
        asset->Load(true);
        GM_LOG_DEBUG("Asset \"{}\" Loaded!", asset->GetPathAsString().c_str());
    }

}

Asset* AssetManager::GetAssetInternal(const std::filesystem::path& path) {
    Asset* asset = mAssets.at(path);

    while (!asset->IsLoaded()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return asset;
}

bool AssetManager::IsAssetLoaded(const std::filesystem::path& path) {
    return mAssets.at(path)->IsLoaded();
}

std::vector<AssetManager::FinishedAsset> AssetManager::GetFinishedAssets() {
    mCommandBufferMutex.lock();
    std::vector<FinishedAsset> ret(mFinishedCommandBuffers.begin(), mFinishedCommandBuffers.end());
    mFinishedCommandBuffers.clear();
    mCommandBufferMutex.unlock();

    return std::move(ret);
}

void AssetManager::QueueWorker() {
    CommandPoolManager::AllocateAssetCommandBuffers(std::this_thread::get_id());

    CommandBuffer* cmd = CommandPoolManager::GetCopyCommandBuffer();

    while (!mShouldStop) {
        mQueueMutex.lock();

        if (mAssetQueue.size() == 0) {
            mQueueMutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        cmd->WaitForFence();
        cmd->Begin(true);

        Asset* currentAsset = mAssetQueue.front();

        mAssetQueue.erase(mAssetQueue.begin());
        mQueueMutex.unlock();

        LoadAssetFunction(currentAsset);

        // Make sure Asset::Load sets Asset::mLoading to false
        GM_ASSERT(currentAsset->mLoading == false);
        cmd->End();

        FinishedAsset finishedAsset;

        finishedAsset.mAsset = currentAsset;
        finishedAsset.mCommandBuffer = cmd;

        mCommandBufferMutex.lock();
        mFinishedCommandBuffers.push_back(finishedAsset);
        mCommandBufferMutex.unlock();
    }

    cmd->WaitForFence();
}

void AssetManager::LoadAssetFunction(Asset* asset) {
    GM_LOG_DEBUG("Loading asset \"{}\"", asset->GetPathAsString().c_str());

    asset->Load(false);

    if (asset->IsLoaded()) {
        GM_LOG_DEBUG("Asset \"{}\" Loaded!", asset->GetPathAsString().c_str());
    } else {
        GM_LOG_DEBUG("Asset \"{}\" Not loaded!", asset->GetPathAsString().c_str());
    }
}

}