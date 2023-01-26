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

#include <Guacamole/vulkan/buffer/buffer.h>
#include <Guacamole/asset/asset.h>
#include <Guacamole/core/math/vec.h>

namespace Guacamole {

struct Vertex {
    vec3 Position;
    vec3 Normal;
    vec2 UV;
};

class Device;
class Mesh : public Asset {
public:
    Mesh(Device* device);
    ~Mesh();

    void CreateVBO(void* data, uint64_t count, uint32_t vertexSize);
    void CreateIBO(void* data, uint64_t count, VkIndexType indexType);

    inline VertexBuffer* GetVBO() const { return mVBO; }
    inline IndexBuffer* GetIBO() const { return mIBO; }
    inline const VkBuffer& GetVBOHandle() const { return mVBO->GetHandle(); }
    inline const VkBuffer& GetIBOHandle() const { return mIBO->GetHandle(); }
    inline VkIndexType GetIndexType() const { return mIBO->GetIndexType(); }
    inline uint32_t GetIndexCount() const { return mIBO->GetCount(); }

    AssetType GetAssetType() const override { return AssetType::Mesh; }

private:
    VertexBuffer* mVBO;
    IndexBuffer* mIBO;
    Device* mDevice;

};

}