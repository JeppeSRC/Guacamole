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
    SceneRenderer(Device* device, Swapchain* swapchain, uint32_t width, uint32_t height);
    ~SceneRenderer();

    void Begin();
    void BeginScene(const CameraComponent& camera, const IdComponent& id);
    void EndScene();
    void End();

    void SubmitMesh(const MeshComponent& mesh, const TransformComponent& transform, const MaterialComponent& material);
private:
    DescriptorSet* GetDescriptorSet(uint32_t frame, UUID id);
    DescriptorSet* AllocateDescriptorSet(uint32_t frame, DescriptorSetLayout* layout, UUID id);

    std::unordered_map<UUID, DescriptorSet> mDescriptorMap;
    std::unordered_map<UUID, UniformBufferSet*> mUniformBuffers;
private:
    Device* mDevice;
    Swapchain* mSwapchain;
    Shader* mShader;
    PipelineLayout* mPipelineLayout;
    Pipeline* mPipeline;
    Renderpass* mRenderpass;

    UniformBufferSet mSceneUniformSet;

    DescriptorPool mDescriptorPool;
    CommandPool mCommandPool;
    CommandBuffer* mStagingCommandBuffer;
    StagingBuffer mStagingBuffer;

    friend class Renderlist;
};

}