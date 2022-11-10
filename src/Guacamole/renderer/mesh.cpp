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

#include "mesh.h"

#include <Guacamole/vulkan/buffer/stagingbuffer.h>

namespace Guacamole {

Mesh::Mesh(const std::filesystem::path& file) 
    : Asset(file, AssetType::Mesh), mVBO(nullptr), 
      mIBO(nullptr), mIndexType(VK_INDEX_TYPE_NONE_KHR) {}

Mesh::~Mesh() {
    delete mVBO;
    delete mIBO;
}

void Mesh::Load() {
    LoadFromFile(mFilePath);
}

void Mesh::Unload() {
    delete mVBO;
    delete mIBO;
}

Mesh::Mesh() 
    : Asset("", AssetType::Mesh), mVBO(nullptr), 
      mIBO(nullptr), mIndexType(VK_INDEX_TYPE_NONE_KHR) {

    mFlags |= AssetFlag_Loaded;
}

void Mesh::CreateVBO(Vertex* data, uint64_t count) {
    GM_ASSERT(mVBO == nullptr);

    uint64_t size = count * sizeof(Vertex);

    mVBO = new Buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size);

    memcpy(StagingBuffer::GetStagingBuffer()->Allocate(size, mVBO), data, size);
}

void Mesh::CreateIBO(void* data, uint64_t count, VkIndexType indexType) {
    GM_ASSERT(mIBO == nullptr);

    uint64_t size = count;

    switch (indexType) {
        case VK_INDEX_TYPE_UINT32:
            size *= 4;
            break;
        case VK_INDEX_TYPE_UINT16:
            size *= 2;
            break;
        case VK_INDEX_TYPE_UINT8_EXT:
            GM_VERIFY_MSG(false, "Unsupported VkIndexType");
            break;
        default:
            GM_VERIFY_MSG(false, "Unsupported VkIndexType");
    }

    mIndexType = indexType;
    mIndexCount = count;
    mIBO = new Buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, size);

    memcpy(StagingBuffer::GetStagingBuffer()->Allocate(size, mIBO), data, size);    
}


void Mesh::LoadFromFile(const std::filesystem::path& path) {
    GM_ASSERT(false);
}

Mesh* Mesh::GenerateQuad() {
    Vertex vertices[4 * 6] = {
        // Front
        {{-0.5, -0.5, 0.5, 1}, {0, 0, 1}, {0, 0}},
        {{ 0.5, -0.5, 0.5, 1}, {0, 0, 1}, {1, 0}},
        {{ 0.5,  0.5, 0.5, 1}, {0, 0, 1}, {1, 1}},
        {{-0.5,  0.5, 0.5, 1}, {0, 0, 1}, {0, 1}},
        // Back
        {{-0.5,  0.5, 0.5, 1}, {0, 0, -1}, {1, 1}},
        {{ 0.5,  0.5, 0.5, 1}, {0, 0, -1}, {0, 1}},
        {{ 0.5, -0.5, 0.5, 1}, {0, 0, -1}, {0, 0}},
        {{-0.5, -0.5, 0.5, 1}, {0, 0, -1}, {1, 0}},
        // Top
        {{-0.5, -0.5, -0.5, 1}, {0, -1, 0}, {0, 0}},
        {{ 0.5, -0.5, -0.5, 1}, {0, -1, 0}, {1, 0}},
        {{ 0.5, -0.5,  0.5, 1}, {0, -1, 0}, {1, 1}},
        {{-0.5, -0.5,  0.5, 1}, {0, -1, 0}, {0, 1}},
        // Bottom
        {{-0.5, 0.5,  0.5, 1}, {0, 1, 0}, {0, 1}},
        {{ 0.5, 0.5,  0.5, 1}, {0, 1, 0}, {1, 1}},
        {{ 0.5, 0.5, -0.5, 1}, {0, 1, 0}, {1, 0}},
        {{-0.5, 0.5, -0.5, 1}, {0, 1, 0}, {0, 0}},
        // Right
        {{0.5, -0.5,  0.5, 1}, {1, 0, 0}, {0, 0}},
        {{0.5, -0.5, -0.5, 1}, {1, 0, 0}, {1, 0}},
        {{0.5,  0.5, -0.5, 1}, {1, 0, 0}, {1, 1}},
        {{0.5,  0.5,  0.5, 1}, {1, 0, 0}, {0, 1}},
        // Left
        {{-0.5,  0.5,  0.5, 1}, {1, 0, 0}, {1, 1}},
        {{-0.5,  0.5, -0.5, 1}, {1, 0, 0}, {0, 1}},
        {{-0.5, -0.5, -0.5, 1}, {1, 0, 0}, {0, 0}},
        {{-0.5, -0.5,  0.5, 1}, {1, 0, 0}, {1, 0}},
    };

    uint16_t indices[6 * 6] = {
        0,  1, 2,  2,  3,  0, 
        4,  5, 6,  6,  7,  4, 
        8,  9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    Mesh* mesh = new Mesh;

    mesh->CreateVBO(vertices, 4 * 6);
    mesh->CreateIBO(indices, 6 * 6, VK_INDEX_TYPE_UINT16);

    mesh->mFlags |= AssetFlag_Loaded;

    return mesh;

    return nullptr;
}

Mesh* Mesh::GeneratePlane() {
    Vertex vertices[4] = {
        {{-0.5, -0.5, 0, 1}, {0, 0, 1}, {0, 0}},
        {{ 0.5, -0.5, 0, 1}, {0, 0, 1}, {1, 0}},
        {{ 0.5,  0.5, 0, 1}, {0, 0, 1}, {1, 1}},
        {{-0.5,  0.5, 0, 1}, {0, 0, 1}, {0, 1}}
    };

    uint16_t indices[6] = {
        0, 1, 2, 2, 3, 0
    };

    Mesh* mesh = new Mesh;

    mesh->CreateVBO(vertices, 4);
    mesh->CreateIBO(indices, 6, VK_INDEX_TYPE_UINT16);

    mesh->mFlags |= AssetFlag_Loaded;

    return mesh;
}

}