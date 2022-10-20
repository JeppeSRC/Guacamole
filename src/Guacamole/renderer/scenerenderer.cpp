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

namespace Guacamole {

SceneRenderer::SceneRenderer(uint32_t width, uint32_t height) {
    mShader = new Shader();
    mShader->AddModule("res/shader/scene.vert", true, ShaderStage::Vertex);
    mShader->AddModule("res/shader/scene.frag", true, ShaderStage::Fragment);
    mShader->Compile();

    mPipelineLayout = new PipelineLayout(mShader->GetDescriptorSetLayouts());

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

    mSceneSet = mShader->AllocateDescriptorSet(0);
}

SceneRenderer::~SceneRenderer() {
    
}

void SceneRenderer::BeginScene(const Camera& camera) {
    mSceneUniformData.mProjection = camera.GetProjection();
    mSceneUniformData.mView = camera.GetView();
}

void SceneRenderer::EndScene() {
    
}

void SceneRenderer::SubmitMesh(const Mesh* mesh, const DescriptorSet* set) {
}
    
}
