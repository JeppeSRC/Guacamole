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
#include <Guacamole/vulkan/shader/texture.h>
#include <Guacamole/renderer/mesh.h>
#include <Guacamole/vulkan/device.h>
#include <Guacamole/util/file.h>

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
std::mutex AssetManager::mAssetMutex;
std::mutex AssetManager::mAssetDataMutex;
std::unordered_map<AssetHandle, Asset*> AssetManager::mAssets;
std::unordered_map<AssetHandle, Asset*> AssetManager::mMemoryAssets;
std::unordered_map<AssetHandle, AssetData*> AssetManager::mAssetData;
std::vector<AssetHandle> AssetManager::mAssetQueue;

void AssetManager::Init(Device* device) {
    mShouldStop = false;
    mDevice = device;
    mLoaderThread = std::thread(&AssetManager::QueueWorker);
}

void AssetManager::Shutdown() {
    mShouldStop = true;
    mLoaderThread.join();

    /*
    for (auto [handle, asset] : mAssets) {
        if (asset->mFlags & AssetFlag_MemoryAsset) {
            if (asset->mFlags & AssetFlag_OwnsMemory) delete asset;
        } else {
            delete asset;
        }
    }*/
}

AssetHandle AssetManager::AddNewAsset(const std::string& filename) {
    AssetData* data = new AssetData;
    data->mFilename = filename;
    data->mType = AssetType::None;
    data->mFlags = 0;
    data->mHostBuffer = nullptr;
    data->mDeviceBuffer = nullptr;

    memset(data->mAssetTypeData, 0, sizeof(data->mAssetTypeData));

    mAssetDataMutex.lock();
    mAssetData[data->mHandle] = data;
    mAssetDataMutex.unlock();
}

Asset* AssetManager::GetAssetInternal(AssetHandle handle) {
    mAssetMutex.lock();
    const auto itr = mAssets.find(handle);
    const auto end = mAssets.end();
    mAssetMutex.unlock();

    if (itr == end) {

        const auto& memitr = mMemoryAssets.find(handle);

        if (memitr == mMemoryAssets.end()) {
            GM_LOG_CRITICAL("Asset [handle: {:08x}] doesn't exist", handle);
            return nullptr;
        }

        return memitr->second;
    }

    Asset* asset = itr->second;
    /*
    while (!asset->IsLoaded()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }*/

    return asset;
}

AssetData* AssetManager::GetAssetDataInternal(AssetHandle handle) {
    mAssetDataMutex.lock();
    auto itr = mAssetData.find(handle);
    auto end = mAssetData.end();
    mAssetDataMutex.unlock();

    GM_ASSERT(itr != end);

    return itr->second;
}

bool AssetManager::IsAssetLoaded(AssetHandle handle) {
    return GetAssetDataInternal(handle)->mFlags & AssetFlag_Loaded;
}

bool AssetManager::IsAssetDataLoaded(AssetHandle handle) {
    return GetAssetDataInternal(handle)->mFlags & AssetFlag_DataLoaded;
}

void AssetManager::QueueWorker() {
    CommandPool pool(mDevice);
    StagingBuffer* buffer = new StagingBuffer(mDevice, 1000000);
    StagingManager::SetCommonStagingBuffer(std::this_thread::get_id(), buffer);

    constexpr uint32_t DEFAULT_CMD_BUFFER_COUNT = 3;

    std::vector<CommandBuffer*> commandBuffers = pool.AllocateCommandBuffers(DEFAULT_CMD_BUFFER_COUNT, true);

    auto GetCmd = [&pool, &commandBuffers]() -> CommandBuffer* {
        for (CommandBuffer* cmd : commandBuffers) {
            VkResult res = cmd->GetSemaphore()->Wait(0);

            switch (res) {
                case VK_SUCCESS:
                    return cmd;
                case VK_TIMEOUT:
                    continue;
                default:
                    VK(res);
            }
        }

        CommandBuffer* newCmd = pool.AllocateCommandBuffer(true);

        commandBuffers.push_back(newCmd);

        return newCmd;
    };

    CommandBuffer* cmd = GetCmd();
    cmd->Begin(true);

    buffer->SetCommandBuffer(cmd);

    while (!mShouldStop) {
        mQueueMutex.lock();

        if (mAssetQueue.size() == 0) {
            mQueueMutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        AssetHandle asset = mAssetQueue.front();

        mAssetQueue.erase(mAssetQueue.begin());
        mQueueMutex.unlock();

        VkPipelineStageFlags stage = LoadAsset(asset);

        if (stage != VK_PIPELINE_STAGE_NONE) {
            StagingManager::SubmitStagingBuffer(buffer, stage);

            cmd = GetCmd();
            cmd->Begin(true);
            buffer->SetCommandBuffer(cmd);
        }
    }
}

VkPipelineStageFlags AssetManager::LoadAsset(AssetHandle handle) {
    AssetData* assetData = GetAssetDataInternal(handle);

    if (assetData->CheckFlags(AssetFlag_DataLoaded)) {
        switch (assetData->mType) {
            case AssetType::Mesh:
                return LoadMeshAsset(handle, assetData, nullptr);
            case AssetType::Texture:
                return LoadTextureAsset(handle, assetData, nullptr);
            case AssetType::Shader:
                return LoadShaderAsset(handle, assetData, nullptr);
            default:
                GM_ASSERT(false);
        }
    }

    File file(assetData->mFilename);
    file.Open();

    AssetHeader hdr;

    file.Read(&hdr, sizeof(AssetHeader), 0);

    if (hdr.Signature != GM_ASSET_SIGNATURE) {
        GM_LOG_WARNING("[AssetManager] Invalid signature, content may not load properly. File \"{}\"", assetData->mFilename);
    }

    GM_VERIFY_MSG(hdr.Version == GM_ASSET_VERSION, "Version missmatch");

    switch (hdr.Type) {
        case AssetType::Mesh:
            return LoadMeshAsset(handle, assetData, &file);
        case AssetType::Texture:
            return LoadTextureAsset(handle, assetData, &file);
        case AssetType::Shader:
            return LoadShaderAsset(handle, assetData, &file);
        default:
            GM_ASSERT(false);
    }
}

VkPipelineStageFlags AssetManager::LoadMeshAsset(AssetHandle handle, AssetData* assetData, File* file) {
    GM_ASSERT(sizeof(AssetData::mAssetTypeData) <= sizeof(MeshHeader));
    MeshHeader* mhdr = (MeshHeader*)assetData->mAssetTypeData;

    if (file) {
        assetData->mType = AssetType::Mesh;

        file->Read(mhdr, sizeof(MeshHeader));

        assetData->mDeviceBuffer = new Buffer(mDevice, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, mhdr->DataSize + 8, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        uint8_t* data = (uint8_t*)assetData->mDeviceBuffer->Map();

        uint64_t vboSize = mhdr->VertexCount * mhdr->VertexSize;
        uint64_t iboSize = mhdr->DataSize - vboSize;
        file->Read(data, vboSize);

        vboSize += 8;
        vboSize &= ~7ULL;

        file->Read(data + vboSize, iboSize);
        file->Close();

        assetData->mDeviceBuffer->Unmap();
    }

    Mesh* mesh = new Mesh(mDevice);

    mesh->CreateVBO(nullptr, mhdr->VertexCount, mhdr->VertexSize);
    mesh->CreateIBO(nullptr, mhdr->IndexCount, mhdr->IndexType);

    CommandBuffer* cmd = StagingManager::GetCommonStagingBuffer()->GetCommandBuffer();

    VkBufferCopy region;

    region.srcOffset = 0;
    region.dstOffset = 0;
    region.size = mesh->GetVBO()->GetSize();

    vkCmdCopyBuffer(cmd->GetHandle(), assetData->mDeviceBuffer->GetHandle(), mesh->GetVBOHandle(), 1, &region);

    region.srcOffset = (region.size + 8) & ~7ULL;
    region.size = mesh->GetIBO()->GetSize();

    vkCmdCopyBuffer(cmd->GetHandle(), assetData->mDeviceBuffer->GetHandle(), mesh->GetIBOHandle(), 1, &region);

    return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
}

VkPipelineStageFlags AssetManager::LoadTextureAsset(AssetHandle handle, AssetData* assetData, File* file) {
    return VK_PIPELINE_STAGE_NONE;
}

VkPipelineStageFlags AssetManager::LoadShaderAsset(AssetHandle handle, AssetData* assetData, File* file) {
    return VK_PIPELINE_STAGE_NONE;
}

}