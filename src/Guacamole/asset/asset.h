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

#include <Guacamole/vulkan/texture.h>
#include <Guacamole/core/uuid.h>

namespace Guacamole {

enum class AssetType {
    Binary,
    Text,
    Shader,
    Texture2D,
    Mesh
};

typedef UUID AssetHandle;

class Asset {
protected:
    AssetHandle mHandle;
    std::filesystem::path mFilePath;
    bool mLoaded;
    bool mLoading;
    AssetType mType;

    Asset(const std::filesystem::path& filePath, AssetType type);
public:
    virtual ~Asset();

    virtual void Load(bool immediate) = 0;
    virtual void Unload() = 0;
    virtual void Unmap() {}

    inline AssetHandle GetHandle() const { return mHandle; }
    inline const std::filesystem::path& GetPath() const { return mFilePath; }
    inline std::string GetPathAsString() const { return mFilePath.string(); }
    inline bool IsLoaded() const { return mLoaded; }
    inline bool IsLoading() const { return mLoading; }

private:
    friend class AssetManager;
};

class Texture2DAsset : public Asset {
public:
    Texture2DAsset(const std::filesystem::path& filePath);
    virtual ~Texture2DAsset();

    void Load(bool immediate) override;
    void Unload() override;
    void Unmap() override;

    inline Texture2D* GetTexture() const { return mTexture; }

private:
    Texture2D* mTexture;
};

}