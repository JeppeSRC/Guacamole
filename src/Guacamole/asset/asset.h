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

#include "assetheader.h"
#include <Guacamole/core/uuid.h>

namespace Guacamole {

using AssetHandle = UUID;

enum AssetFlags {
    AssetFlag_Loaded = 0x01, 
    AssetFlag_Loading = 0x02, 
    AssetFlag_DataLoaded = 0x04,
    AssetFlag_MemoryAsset = 0x08,
    AssetFlag_HostData = 0x10,
    AssetFlag_DeviceData = 0x20
};

class Asset {
protected:
    AssetHandle mHandle;

    friend class AssetManager;
public:
    virtual ~Asset() {}

    inline virtual AssetType GetAssetType() const = 0;
    static AssetType GetStaticType() { return AssetType::None; }

    inline AssetHandle GetHandle() const { return mHandle; }
};

class AssetData {
public:
    AssetHandle mHandle;
    std::string mFilename;
    AssetType mType;
    uint32_t mFlags;

    void* mHostBuffer;
    Buffer* mDeviceBuffer;

    uint8_t mAssetTypeData[48];

    inline bool CheckFlags(uint32_t flags) const { return (mFlags & flags) == flags; }

    friend class AssetManager;
};


}