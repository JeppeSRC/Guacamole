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

#include <Guacamole/vulkan/context.h>
#include <Guacamole/vulkan/swapchain.h>
#include <Guacamole/vulkan/renderpass.h>
#include <Guacamole/vulkan/pipeline.h>
#include <Guacamole/vulkan/buffer.h>
#include <Guacamole/vulkan/texture.h>
#include <Guacamole/vulkan/descriptor.h>
#include <Guacamole/vulkan/shader.h>
#include <Guacamole/asset/assetmanager.h>
#include <Guacamole/renderer/mesh.h>
#include <time.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Guacamole/core/application.h>
#include <Guacamole/util/timer.h>

using namespace Guacamole;

class TestApp : public Application {
public:
    TestApp(ApplicationSpec& spec) : Application(spec) {}

    void OnInit() override {
        spdlog::set_level(spdlog::level::debug);

        WindowSpec spec;

        spec.Width = 1280;
        spec.Height = 720;
        spec.Windowed = true;
        spec.Title = "Dope TItle";

        Init(spec);
    }

    void OnUpdate(float ts) override {

    }

    void OnRender() override {

    }

    void OnShutdown() override {

    }

    bool OnKeyPressed(KeyPressedEvent* e) override {
        return false;
    }

    bool OnKeyReleased(KeyReleasedEvent* e) override {
        return false;
    }

    bool OnButtonPressed(ButtonPressedEvent* e) override {
        return false;
    }

    bool OnButtonReleased(ButtonReleasedEvent* e) override {
        return false;
    }

    bool OnMouseMoved(MouseMovedEvent* e) override {
        return false;
    }

private:

};

int main() {
    ApplicationSpec appSpec;

    TestApp app(appSpec);

    app.Run();

    return 0;
    
    spdlog::set_level(spdlog::level::debug);

    WindowSpec spec;

    spec.Width = 1280;
    spec.Height = 720;
    spec.Windowed = true;
    spec.Title = "Dope TItle";
    
    
    Guacamole::Window window(spec);

    Context::Init(&window);
    CommandPoolManager::AllocateCopyCommandBuffers(std::this_thread::get_id(), 2);
    AssetManager::Init();

    Swapchain::Init(&window);

    Vertex vertices[]{
        {glm::vec4(-0.5, -0.5, 0, 1), glm::vec3(0, 0, 1),glm::vec2(0, 0)},
        {glm::vec4( 0.5, -0.5, 0, 1), glm::vec3(0, 0, 1),glm::vec2(1, 0)},
        {glm::vec4( 0.5,  0.5, 0, 1), glm::vec3(0, 0, 1),glm::vec2(1, 1)},
        {glm::vec4(-0.5,  0.5, 0, 1), glm::vec3(0, 0, 1),glm::vec2(0, 1)}
    };

    uint32_t indices[]{ 0, 1, 2, 2, 3, 0 };

    {
        Buffer vbo(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sizeof(vertices), vertices);
        Buffer ibo(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, sizeof(indices), indices);

        Shader shader;
        shader.AddModule("build/res/shader.vert", true, ShaderStage::Vertex);
        shader.AddModule("build/res/shader.frag", true, ShaderStage::Fragment);
        shader.Compile();

        Texture2D tex(100, 100, VK_FORMAT_R32G32B32A32_SFLOAT);

        glm::vec4 colors[100 * 100];

        for (uint32_t y = 0; y < 100; y++) {
            for (uint32_t x = 0; x < 100; x++) {
                colors[y * 100 + x] = glm::vec4((float)x / 100.0f, 0, (float)y / 100.0f, 1);
            }
        }

        tex.WriteDataImmediate(colors, sizeof(colors));

        AssetHandle texHandle = AssetManager::AddMemoryAsset(&tex);
        AssetHandle sheetHandle = AssetManager::AddAsset(new Texture2D("build/res/sheet.png"), false);
        //AssetManager::AddAsset(new Mesh("res/mesh.obj"), false);
        AssetHandle objHandle = AssetManager::AddMemoryAsset(Mesh::GeneratePlane(true));

        BasicRenderpass pass;

        PipelineLayout pipelineLayout(shader.GetDescriptorSetLayout(0));

        GraphicsPipelineInfo gInfo;

        gInfo.mWidth = spec.Width;
        gInfo.mHeight = spec.Height;
        gInfo.mVertexInputBindings.push_back({ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        gInfo.mVertexInputAttributes = shader.GetVertexInputLayout({ {0, {0, 1, 2 } } });
        gInfo.mPipelineLayout = &pipelineLayout;
        gInfo.mRenderpass = &pass;
        gInfo.mShader = &shader;
        
        GraphicsPipeline gPipeline(gInfo);

        DescriptorSet* set = shader.AllocateDescriptorSets(0, 1)[0];
        VkDescriptorSet setHandle = set->GetHandle();

        Buffer uniform(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, set->GetLayout()->GetUniformBufferSize(0));

        BasicSampler sampler;

        VkDescriptorBufferInfo bInfo;

        bInfo.buffer = uniform.GetHandle();
        bInfo.offset = 0;
        bInfo.range = uniform.GetSize();

        VkWriteDescriptorSet wSet[2];

        wSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wSet[0].pNext = nullptr;
        wSet[0].dstSet = setHandle;
        wSet[0].dstBinding = 0;
        wSet[0].dstArrayElement = 0;
        wSet[0].descriptorCount = 1;
        wSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        wSet[0].pImageInfo = nullptr;
        wSet[0].pBufferInfo = &bInfo;
        wSet[0].pTexelBufferView = nullptr;

        VkDescriptorImageInfo iInfo;

        iInfo.sampler = sampler.GetHandle();
        iInfo.imageView = AssetManager::GetAsset<Texture>(sheetHandle)->GetImageViewHandle();
        iInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        wSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wSet[1].pNext = nullptr;
        wSet[1].dstSet = setHandle;
        wSet[1].dstBinding = 1;
        wSet[1].dstArrayElement = 0;
        wSet[1].descriptorCount = 1;
        wSet[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        wSet[1].pImageInfo = &iInfo;
        wSet[1].pBufferInfo = nullptr;
        wSet[1].pTexelBufferView = nullptr;
        
        vkUpdateDescriptorSets(Context::GetDeviceHandle(), 2, wSet, 0, nullptr);

        void* mem = uniform.Map();

        glm::vec4 color(1, 1, 1, 1);

        memcpy(mem, &color, sizeof(glm::vec4));

        uniform.StageCopy(true);

        while (!window.ShouldClose()) {
            auto start = std::chrono::high_resolution_clock::now();
            Swapchain::Begin();
            CommandBuffer* cmd = Swapchain::GetPrimaryCommandBuffer();

            VkCommandBuffer handle = cmd->GetHandle();

            vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, gPipeline.GetHandle());

            pass.Begin(cmd);

            Mesh* mesh = AssetManager::GetAsset<Mesh>(objHandle);

            VkBuffer vboHandle = mesh->GetVBOHandle();
            VkBuffer iboHandle = mesh->GetIBOHandle();

            VkDeviceSize offset = 0;
            
            vkCmdBindVertexBuffers(handle, 0, 1, &vboHandle, &offset);
            vkCmdBindIndexBuffer(handle, iboHandle, 0, mesh->GetIndexType());
            vkCmdBindDescriptorSets(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.GetHandle(), 0, 1, &setHandle, 0, 0);

            vkCmdDrawIndexed(handle, 6, 1, 0, 0, 0);

            pass.End(cmd);

            Swapchain::Present();

          /*  auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            float ms = float(duration) / 1000.0f;

            std::cout << "Frame Time: " << ms << "ms FPS: " << 1000.0f / ms << "\n";*/
        }

        Swapchain::WaitForAllCommandBufferFences();
    }

    AssetManager::Shutdown();
    Swapchain::Shutdown();
    CommandPoolManager::Shutdown();
    Context::Shutdown();

    return 0;
}