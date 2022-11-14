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

#include <glm/glm.hpp>

#include <Guacamole/vulkan/buffer/buffer.h>
#include <Guacamole/asset/asset.h>

namespace Guacamole {

struct Vertex {
    glm::vec4 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
};

class Mesh : public Asset {
public:
    Mesh(const std::filesystem::path& file);
    ~Mesh();

    void Load() override;
    void Unload() override;

    inline VertexBuffer* GetVBO() const { return mVBO; }
    inline IndexBuffer* GetIBO() const { return mIBO; }
    inline const VkBuffer& GetVBOHandle() const { return mVBO->GetHandle(); }
    inline const VkBuffer& GetIBOHandle() const { return mIBO->GetHandle(); }
    inline VkIndexType GetIndexType() const { return mIBO->GetIndexType(); }
    inline uint32_t GetIndexCount() const { return mIBO->GetCount(); }

private:
    Mesh();

    void CreateVBO(Vertex* data, uint64_t count);
    void CreateIBO(void* data, uint64_t count, VkIndexType indexType);
    void LoadFromFile(const std::filesystem::path& path);

    VertexBuffer* mVBO;
    IndexBuffer* mIBO;

public:
    static Mesh* GenerateQuad();
    static Mesh* GeneratePlane();
};

}