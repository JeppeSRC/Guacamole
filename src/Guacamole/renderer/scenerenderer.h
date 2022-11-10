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

#include "renderer.h"
#include "mesh.h"
#include "camera.h"

#include <Guacamole/vulkan/shader/descriptor.h>
#include <Guacamole/vulkan/buffer/stagingbuffer.h>
#include <Guacamole/vulkan/buffer/buffer.h>

namespace Guacamole {



struct SceneUniformData {
    SceneUniformData() : 
        mUniformBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                        sizeof(Data), 
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {}

    Buffer mUniformBuffer;
    DescriptorSet* mSceneSet;

    struct Data {
        glm::mat4 mProjection;
        glm::mat4 mView;
    } *mData;
};

struct SceneMaterialData {
    glm::vec4 mColor;
};

struct SceneModelData {
    glm::mat4 mModelMatrix;
};

class Scene;
class SceneRenderer {
public:
    SceneRenderer(uint32_t width, uint32_t height);
    ~SceneRenderer();

    void Begin(Scene* scene);
    void BeginScene(const Camera& camera);
    void EndScene();
    void End();

    void SubmitMesh(const Mesh* mesh, const DescriptorSet* modelSet, const DescriptorSet* materialSet);

    inline StagingBuffer* GetStagingBuffer() { return &mStagingBuffer; }
private:
    Shader* mShader;
    PipelineLayout* mPipelineLayout;
    Pipeline* mPipeline;
    Renderpass* mRenderpass;

    StagingBuffer mStagingBuffer;
    SceneUniformData mSceneUniformData;

    Scene* mScene;
};

}