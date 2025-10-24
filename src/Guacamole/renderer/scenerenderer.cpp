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

#include "scenerenderer.h"

#include <Guacamole/asset/assetmanager.h>
#include <Guacamole/vulkan/swapchain.h>
#include <Guacamole/scene/scene.h>
#include <Guacamole/core/application.h>
#include <Guacamole/renderer/material.h>


namespace Guacamole {

SceneRenderer::SceneRenderer(Scene* scene) : 
        mScene(scene), mDevice(scene->GetDevice()), mSwapchain(scene->GetSwapchain()),
        mStagingBuffer(scene->GetDevice(), 1024 * 10), mSceneUniformSet(scene->GetDevice(), 
        scene->GetSwapchain()->GetFramesInFlight()), mSampler(scene->GetDevice()),
        mCommandPool(scene->GetDevice())  {

    AssetHandle vertHandle = AssetManager::AddAsset(new Shader::Source("res/shader/scene.vert", false, ShaderStage::Vertex), false);
    AssetHandle fragHandle = AssetManager::AddAsset(new Shader::Source("res/shader/scene.frag", false, ShaderStage::Fragment), false);

    mShader = new Shader(mDevice);
    mShader->AddModule(vertHandle, ShaderStage::Vertex);
    mShader->AddModule(fragHandle, ShaderStage::Fragment);
    mShader->Compile();

    mPipelineLayout = new PipelineLayout(mDevice, mShader->GetDescriptorSetLayouts(), mShader->GetPushConstants());

    mRenderpass = new BasicRenderpass(mSwapchain, mDevice);

    GraphicsPipelineInfo gInfo;

    Window* window = mScene->GetApplication()->GetWindow();

    gInfo.mWidth = window->GetWidth();
    gInfo.mHeight = window->GetHeight();
    gInfo.mPipelineLayout = mPipelineLayout;
    gInfo.mRenderpass = mRenderpass;
    gInfo.mShader = mShader;
    gInfo.mVertexInputAttributes = mShader->GetVertexInputLayout({ { 0, { 0, 1, 2 }}});
    gInfo.mVertexInputBindings.push_back({0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX});

    mPipeline = new GraphicsPipeline(mDevice, gInfo);

    mSceneUniformSet.Create(0, sizeof(SceneData));

    constexpr uint32_t descriptorCount = 100;

    for (uint32_t i = 0; i < mSwapchain->GetFramesInFlight(); i++) {
        mDescriptorPools.push_back(new DescriptorPool(mDevice, descriptorCount));
    }

    for (uint32_t i = 0; i < descriptorCount; i++) {
        UniformBufferSet* set = new UniformBufferSet(mDevice, mSwapchain->GetFramesInFlight());
        mUniformBufferSetPool.push_back(set);

        set->Create(1, sizeof(vec4));
    }

    mStagingCommandBuffer = mCommandPool.AllocateCommandBuffer(true);
    mStagingBuffer.SetCommandBuffer(mStagingCommandBuffer);
}

SceneRenderer::~SceneRenderer() {
    for (UniformBufferSet* set : mUniformBufferSetPool)
        delete set;

    for (DescriptorPool* pool : mDescriptorPools) 
        delete pool;

    delete mStagingCommandBuffer;
    delete mPipeline;
    delete mPipelineLayout;
    delete mRenderpass;
    delete mShader;
}

void SceneRenderer::Begin() {
    CommandBuffer* cmd = mSwapchain->GetRenderCommandBuffer();
    cmd->Wait();
    cmd->Begin(true);

    mStagingBuffer.Begin();

    //mRenderlist.Begin(cmd, Swapchain::GetCurrentImageIndex());
    mUniformBufferSetIndex = 0;
}

void SceneRenderer::BeginScene(const Camera& camera) {
    uint32_t frame = mSwapchain->GetCurrentImageIndex();

    DescriptorPool* descPool = mDescriptorPools[frame];
    descPool->Reset();

    DescriptorSet set = descPool->AllocateDescriptorSet(mShader->GetDescriptorSetLayout(0));

    UniformBuffer* buffer = mSceneUniformSet.Get(frame, 0);
    
    VkDescriptorBufferInfo bInfo;
    bInfo.buffer = buffer->GetHandle();
    bInfo.offset = 0;
    bInfo.range = buffer->GetSize();

    VkWriteDescriptorSet write;
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.dstArrayElement = 0;
    write.dstBinding = 0;
    write.dstSet = set.GetHandle();
    write.pBufferInfo = &bInfo;

    vkUpdateDescriptorSets(mDevice->GetHandle(), 1, &write, 0, 0);

    SceneData* data = (SceneData*)mStagingBuffer.Allocate(sizeof(SceneData), buffer);

    data->mProjection = camera.GetProjection();
    data->mView = camera.GetView();

    CommandBuffer* cmd = mSwapchain->GetRenderCommandBuffer();

    const VkViewport& viewport = camera.GetViewport();

    VkRect2D rect;

    rect.offset.x = 0;
    rect.offset.y = 0;
    rect.extent.width = (uint32_t)viewport.width;
    rect.extent.height = (uint32_t)viewport.height;
    
    vkCmdSetViewport(cmd->GetHandle(), 0, 1, &viewport);
    vkCmdSetScissor(cmd->GetHandle(), 0, 1, &rect);

    Renderer::BeginRenderpass(cmd, mRenderpass);
    vkCmdBindDescriptorSets(cmd->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(), 0, 1, &set.GetHandle(), 0, 0);
}

void SceneRenderer::EndScene() {
    //mRenderlist.End();
    //mRenderlist.Execute();

    Renderer::EndRenderpass(mSwapchain->GetRenderCommandBuffer(), mRenderpass);
}

void SceneRenderer::End() {
    StagingManager::SubmitStagingBuffer(&mStagingBuffer, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
}

void SceneRenderer::SubmitMesh(const MeshComponent& mesh, const TransformComponent& transform, const MaterialComponent& material) {
    CommandBuffer* cmd = mSwapchain->GetRenderCommandBuffer();
    VkCommandBuffer cmdHandle = cmd->GetHandle();

    Mesh* meshAsset = AssetManager::GetAsset<Mesh>(mesh.mMesh);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmdHandle, 0, 1, &meshAsset->GetVBOHandle(), &offset);
    vkCmdBindIndexBuffer(cmdHandle, meshAsset->GetIBOHandle(), 0, meshAsset->GetIndexType());

    mat4 trans = transform.GetTransform();
    vkCmdPushConstants(cmdHandle, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), &trans);

    Material* materialAsset = AssetManager::GetAsset<Material>(material.mMaterial);
    Texture2D* tex = AssetManager::GetAsset<Texture2D>(materialAsset->mTextureHandle);

    DescriptorPool* descPool = mDescriptorPools[mSwapchain->GetCurrentImageIndex()];
    DescriptorSet set = descPool->AllocateDescriptorSet(mShader->GetDescriptorSetLayout(1));
    VkDescriptorSet setHandle = set.GetHandle();
    UniformBufferSet* bufferSet = mUniformBufferSetPool[mUniformBufferSetIndex++];
    UniformBuffer* buffer = bufferSet->Get(mSwapchain->GetCurrentImageIndex(), 1);

    VkDescriptorBufferInfo bInfo;
    bInfo.buffer = buffer->GetHandle();
    bInfo.offset = 0;
    bInfo.range = buffer->GetSize();

    VkDescriptorImageInfo iInfo;

    iInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    iInfo.imageView = tex->GetImageViewHandle();
    iInfo.sampler = mSampler.GetHandle();

    VkWriteDescriptorSet write[2];
    write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write[0].pNext = nullptr;
    write[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write[0].descriptorCount = 1;
    write[0].dstArrayElement = 0;
    write[0].dstBinding = 0;
    write[0].dstSet = setHandle;
    write[0].pImageInfo = &iInfo;

    write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write[1].pNext = nullptr;
    write[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write[1].descriptorCount = 1;
    write[1].dstArrayElement = 0;
    write[1].dstBinding = 1;
    write[1].dstSet = setHandle;
    write[1].pBufferInfo = &bInfo;

    vkUpdateDescriptorSets(mDevice->GetHandle(), 2, write, 0, 0);
    vkCmdBindDescriptorSets(cmdHandle, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(), 1, 1, &setHandle, 0, 0);

    memcpy(mStagingBuffer.Allocate(sizeof(vec4), buffer), &materialAsset->mAlbedo, sizeof(mat4));

    vkCmdBindPipeline(cmdHandle, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->GetHandle());
    vkCmdDrawIndexed(cmdHandle, meshAsset->GetIndexCount(), 1, 0, 0, 0);

}

}
