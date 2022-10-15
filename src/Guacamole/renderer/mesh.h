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

#include <glm/glm.hpp>

#include <Guacamole/vulkan/buffer.h>

namespace Guacamole {

struct Vertex {
    glm::vec4 Position;
    glm::vec4 Color;
    glm::vec3 Normal;
    glm::vec2 UV;
};

class Mesh {
public:
    ~Mesh();

    static Mesh* LoadFromFile(const std::filesystem::path& path, bool immediate = false);
    static Mesh* GenerateQuad(bool immediate = false);
    static Mesh* GeneratePlane(bool immediate = false);

    inline Buffer* GetVBO() const { return mVBO; }
    inline Buffer* GetIBO() const { return mIBO; }
    inline VkBuffer GetVBOHandle() const { return mVBO->GetHandle(); }
    inline VkBuffer GetIBOHandle() const { return mIBO->GetHandle(); }

private:
    Mesh();

    void CreateVBO(Vertex* data, uint64_t count);
    void CreateIBO(void* data, uint64_t count, VkIndexType indexType);

    Buffer* mVBO;
    Buffer* mIBO;
    VkIndexType mIndexType;
};

}