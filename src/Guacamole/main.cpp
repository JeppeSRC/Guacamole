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
#include <Guacamole/core/descriptor.h>
#include <glm/glm.hpp>

using namespace Guacamole;

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoord;
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

    {
        BasicRenderpass pass;

        DescriptorPool pool(10);
        DescriptorSetLayout descriptorLayout;
        PipelineLayout pipelineLayout(&descriptorLayout);

        GraphicsPipelineInfo gInfo;

        gInfo.Width = spec.Width;
        gInfo.Height = spec.Height;
        gInfo.VertexInputBindings.push_back({ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        gInfo.VertexInputAttributes.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 });
        gInfo.VertexInputAttributes.push_back({ 1, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(glm::vec2)});
        gInfo.PipelineLayout = &pipelineLayout;
        gInfo.Renderpass = &pass;
        

        while (!window.ShouldClose()) {
            glfwPollEvents();
        }

    }

    Swapchain::Shutdown();

    Context::Shutdown();

    return 0;
}