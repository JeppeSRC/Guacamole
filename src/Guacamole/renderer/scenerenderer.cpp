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
#include <Guacamole/vulkan/context.h>
#include <Guacamole/vulkan/buffer/commandpoolmanager.h>
#include <Guacamole/scene/scene.h>

namespace Guacamole {

SceneRenderer::SceneRenderer(uint32_t width, uint32_t height) : mStagingBuffer(1024 * 10) {
    mShader = new Shader;
    mShader->AddModule("res/shader/scene.vert", true, ShaderStage::Vertex);
    mShader->AddModule("res/shader/scene.frag", true, ShaderStage::Fragment);
    mShader->Compile();

    mPipelineLayout = new PipelineLayout(mShader->GetDescriptorSetLayouts(), mShader->GetPushConstants());

    mRenderpass = new BasicRenderpass;

    GraphicsPipelineInfo gInfo;

    gInfo.mWidth = width;
    gInfo.mHeight = height;
    gInfo.mPipelineLayout = mPipelineLayout;
    gInfo.mRenderpass = mRenderpass;
    gInfo.mShader = mShader;
    gInfo.mVertexInputAttributes = mShader->GetVertexInputLayout({ { 0, { 0, 1, 2 }}});
    gInfo.mVertexInputBindings.push_back({0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX});

    mPipeline = new GraphicsPipeline(gInfo);

    mSceneUniformData.mSceneSet = mShader->AllocateDescriptorSet(0);

    VkWriteDescriptorSet wSet;

    VkDescriptorBufferInfo bInfo;

    bInfo.buffer = mSceneUniformData.mUniformBuffer.GetHandle();
    bInfo.offset = 0;
    bInfo.range = sizeof(mSceneUniformData.mData);

    wSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    wSet.pNext = nullptr;
    wSet.dstSet = mSceneUniformData.mSceneSet->GetHandle();
    wSet.dstBinding = 0;
    wSet.dstArrayElement = 0;
    wSet.descriptorCount = 1;
    wSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    wSet.pImageInfo = nullptr;
    wSet.pBufferInfo = &bInfo;
    wSet.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(Context::GetDeviceHandle(), 1, &wSet, 0, nullptr);

}

SceneRenderer::~SceneRenderer() {
    delete mPipeline;
    delete mPipelineLayout;
    delete mRenderpass;
    delete mShader;
}

void SceneRenderer::Begin(Scene* scene) {
    mScene = scene;
    mStagingBuffer.Begin();
    CommandPoolManager::GetRenderCommandBuffer()->Begin(true);
}

void SceneRenderer::BeginScene(const Camera& camera) {
    mSceneUniformData.mData = (SceneUniformData::Data*)mStagingBuffer.Allocate(sizeof(SceneUniformData::Data), &mSceneUniformData.mUniformBuffer);
    mSceneUniformData.mData->mProjection = camera.GetProjection();
    mSceneUniformData.mData->mView = camera.GetView();

    CommandBuffer* cmd = CommandPoolManager::GetRenderCommandBuffer();

    cmd->WaitForFence();
    cmd->Begin(true);

    Renderer::BindPipeline(cmd, mPipeline);
    Renderer::BeginRenderpass(cmd, mRenderpass);

    vkCmdBindDescriptorSets(cmd->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(), 0, 1, &mSceneUniformData.mSceneSet->GetHandle(), 0, nullptr);
}

void SceneRenderer::EndScene() {
    Renderer::EndRenderpass(CommandPoolManager::GetRenderCommandBuffer(), mRenderpass);
}

void SceneRenderer::End() {
    StagingManager::SubmitStagingBuffer(&mStagingBuffer, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
}

void SceneRenderer::SubmitMesh(const Mesh* mesh, const DescriptorSet* modelSet, const DescriptorSet* materialSet) {
    CommandBuffer* cmd = CommandPoolManager::GetRenderCommandBuffer();

    VkDescriptorSet sets[2] = {modelSet->GetHandle(), materialSet->GetHandle()};

    vkCmdBindDescriptorSets(cmd->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(), 1, 2, sets, 0, nullptr);

    VkDeviceSize offset = 0;

    vkCmdBindIndexBuffer(cmd->GetHandle(), mesh->GetIBOHandle(), 0, mesh->GetIndexType());
    vkCmdBindVertexBuffers(cmd->GetHandle(), 0, 1, &mesh->GetVBOHandle(), &offset);

    vkCmdDrawIndexed(cmd->GetHandle(), mesh->GetIndexCount(), 0, 0, 0, 0);

}
    
}
