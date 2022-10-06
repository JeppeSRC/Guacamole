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

#include "asset.h"

#include "../util/util.h"

namespace Guacamole {

Asset::Asset(const std::filesystem::path& filePath, AssetType type) 
    : mHandle(), mFilePath(filePath), mLoaded(false), mLoading(false), mType(type) {}

Asset::~Asset() {
  
}

    
Texture2DAsset::Texture2DAsset(const std::filesystem::path& filePath) 
    : Asset(filePath, AssetType::Texture2D), mTexture(nullptr) {}

Texture2DAsset::~Texture2DAsset() {
    if (mLoaded) {
        Unload();
    }
}

void Texture2DAsset::Load(bool immediate) {
    GM_ASSERT_MSG(!mLoaded, "Load may only be called if the asset isn't loaded");

    mTexture = Texture2D::LoadImageFromFile(mFilePath, immediate);

    if (mTexture == nullptr) {
        mLoaded = false;
    } else {
        mLoaded = true;
    }

    mLoading = false;
}

void Texture2DAsset::Unload() {
    delete mTexture;
    mTexture = nullptr;
    mLoaded = false;
}

void Texture2DAsset::Unmap() {
    mTexture->Unmap();
}

}
