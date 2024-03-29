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

#include <filesystem>

#include <Guacamole/core/uuid.h>

namespace Guacamole {

enum class AssetType {
    Binary,
    Text,
    Mesh,
    ShaderSource,
    Scene,
    Texture,
    Audio,
    Material
};

using AssetHandle = UUID;

enum AssetFlags {
    AssetFlag_Loaded = 0x01, // asset is loaded
    AssetFlag_MemoryAsset = 0x02, // asset is loaded from memory
    AssetFlag_OwnsMemory = 0x04, // asset owns and controls memory (asset manager will free the memory)
    AssetFlag_Loading = 0x08, // asset is in the asset queue and is wating to be loaded    
};

class Asset {
protected:
    AssetHandle mHandle;
    std::filesystem::path mFilePath;
    AssetType mType;
    uint32_t mFlags;

    Asset(const std::filesystem::path& filePath, AssetType type);
public:
    virtual ~Asset();

    // Returns true if the commandbuffer/stagingbuffer was used
    virtual bool Load();
    virtual void Unload();

    inline AssetHandle GetHandle() const { return mHandle; }
    inline const std::filesystem::path& GetPath() const { return mFilePath; }
    inline std::string GetPathAsString() const { return mFilePath.string(); }
    inline bool IsLoaded() const { return mFlags & AssetFlag_Loaded; }
    inline bool IsLoading() const { return mFlags & AssetFlag_Loading; }
    inline AssetType GetType() const { return mType; }
    inline uint32_t GetFlags() const { return mFlags; }

private:
    friend class AssetManager;
};


}