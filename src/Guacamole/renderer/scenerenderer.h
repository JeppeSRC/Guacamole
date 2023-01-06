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
#include <Guacamole/vulkan/shader/sampler.h>
#include <Guacamole/vulkan/buffer/stagingbuffer.h>
#include <Guacamole/vulkan/buffer/buffer.h>
#include <Guacamole/vulkan/buffer/uniformbufferset.h>
#include <Guacamole/vulkan/swapchain.h>
#include <Guacamole/scene/components.h>

namespace Guacamole {

class Scene;
class SceneRenderer {
private:
struct SceneData {
    mat4 mProjection;
    mat4 mView;
};

public:
    SceneRenderer(Scene* scene);
    ~SceneRenderer();

    void Begin();
    void BeginScene(const Camera& camera);
    void EndScene();
    void End();

    void SubmitMesh(const MeshComponent& mesh, const TransformComponent& transform, const MaterialComponent& material);
private:
    std::vector<DescriptorPool*> mDescriptorPools;
    
    // Temporary just to get something going
    uint32_t mUniformBufferSetIndex;
    std::vector<UniformBufferSet*> mUniformBufferSetPool;
    BasicSampler mSampler;

private:
    Scene* mScene;
    Device* mDevice;
    Swapchain* mSwapchain;
    Shader* mShader;
    PipelineLayout* mPipelineLayout;
    Pipeline* mPipeline;
    Renderpass* mRenderpass;

    UniformBufferSet mSceneUniformSet;

    CommandPool mCommandPool;
    CommandBuffer* mStagingCommandBuffer;
    StagingBuffer mStagingBuffer;

    friend class Renderlist;
};

}