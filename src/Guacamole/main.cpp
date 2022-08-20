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

#include <Guacamole/core/context.h>
#include <Guacamole/core/swapchain.h>
#include <Guacamole/core/renderpass.h>
#include <Guacamole/core/pipeline.h>
#include <Guacamole/core/buffer.h>
#include <Guacamole/core/descriptor.h>
#include <Guacamole/core/shader.h>
#include <time.h>
#include <glm/glm.hpp>

using namespace Guacamole;

struct Vertex {
    glm::vec3 Position;
    glm::vec4 Color;
};

int main() {
    
    spdlog::set_level(spdlog::level::debug);
    Context::Init();

    WindowSpec spec;

    spec.Width = 1280;
    spec.Height = 720;
    spec.Windowed = true;
    spec.Title = "Dope TItle";
    
    Guacamole::Window window(spec);

    Swapchain::Init(&window);

    Vertex vertices[]{
        {glm::vec3(-0.5, -0.5, 0), glm::vec4(1, 0, 0, 1)},
        {glm::vec3( 0.5, -0.5, 0), glm::vec4(0, 1, 0, 1)},
        {glm::vec3( 0.5,  0.5, 0), glm::vec4(0, 1, 1, 1)},
        {glm::vec3(-0.5,  0.5, 0), glm::vec4(0, 0, 1, 1)}
    };

    uint32_t indices[]{ 0, 1, 2, 2, 3, 0 };

    {
        Buffer vbo(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sizeof(vertices), vertices);
        Buffer ibo(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, sizeof(indices), indices);

        Shader vert("res/shader.vert", true, ShaderStage::Vertex);
        Shader frag("res/shader.frag", true, ShaderStage::Fragment);

        BasicRenderpass pass;

        DescriptorPool pool(10);
        DescriptorSetLayout descriptorLayout;// = vert.GetDescriptorSetLayout(0);
        PipelineLayout pipelineLayout(&descriptorLayout);

        GraphicsPipelineInfo gInfo;

        gInfo.Width = spec.Width;
        gInfo.Height = spec.Height;
        gInfo.VertexInputBindings.push_back({ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        gInfo.VertexInputAttributes = vert.GetVertexInputLayout({ {0, {0, 1} } });
        gInfo.PipelineLayout = &pipelineLayout;
        gInfo.Renderpass = &pass;
        gInfo.VertexShader = &vert;
        gInfo.FragmentShader = &frag;
        
        GraphicsPipeline gPipeline(gInfo);

        while (!window.ShouldClose()) {
            auto start = std::chrono::high_resolution_clock::now();
            Swapchain::Begin();
            CommandBuffer* cmd = Swapchain::GetPrimaryCommandBuffer(0);

            VkCommandBuffer handle = cmd->GetHandle();

            glfwPollEvents();

            vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, gPipeline.GetHandle());

            pass.Begin(cmd);

            VkBuffer vboHandle = vbo.GetHandle();
            VkBuffer iboHandle = ibo.GetHandle();

            VkDeviceSize offset = 0;

            vkCmdBindVertexBuffers(handle, 0, 1, &vboHandle, &offset);
            vkCmdBindIndexBuffer(handle, iboHandle, 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(handle, 6, 1, 0, 0, 0);

            pass.End(cmd);


            Swapchain::Present();

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            float ms = float(duration) / 1000.0f;

            std::cout << "Frame Time: " << ms << "ms FPS: " << 1000.0f / ms << "\n";
        }

        Swapchain::WaitForAllCommandBufferFences();
    }

    Swapchain::Shutdown();

    Context::Shutdown();

    return 0;
}