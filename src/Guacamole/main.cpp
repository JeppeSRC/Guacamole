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
#include <Guacamole/vulkan/device.h>
#include <Guacamole/vulkan/swapchain.h>
#include <Guacamole/vulkan/renderpass.h>
#include <Guacamole/vulkan/pipeline/pipeline.h>
#include <Guacamole/vulkan/buffer/buffer.h>
#include <Guacamole/vulkan/buffer/commandbuffer.h>
#include <Guacamole/vulkan/shader/texture.h>
#include <Guacamole/vulkan/shader/descriptor.h>
#include <Guacamole/vulkan/shader/shader.h>
#include <Guacamole/asset/assetmanager.h>
#include <Guacamole/renderer/mesh.h>
#include <Guacamole/scene/scene.h>
#include <Guacamole/scene/entity.h>
#include <time.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Guacamole/core/application.h>
#include <Guacamole/core/input.h>
#include <Guacamole/util/timer.h>
#include <Guacamole/vulkan/buffer/stagingbuffer.h>
#include <Guacamole/renderer/meshfactory.h>
#include <Guacamole/renderer/material.h>
#include <Guacamole/scene/nativescript.h>

using namespace Guacamole;

class TestApp : public Application {
public:
    TestApp(ApplicationSpec& spec) : Application(spec) {}

    void OnInit() override {
        spdlog::set_level(spdlog::level::debug);

        WindowSpec windowSpec;

        windowSpec.Width = 1280;
        windowSpec.Height = 720;
        windowSpec.Windowed = true;
        windowSpec.Title = "Dope TItle";

        AppInitSpec initSpec;

        initSpec.appName = "TestApp";
        initSpec.deviceIndex = ~0;

        Init(windowSpec, initSpec);

        mScene = new Scene(this);

        AssetHandle texAsset = AssetManager::AddAsset(new Texture2D(mMainDevice, "build/res/sheet.png"), false);
        AssetHandle matAsset = AssetManager::AddMemoryAsset(new Material(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texAsset), true);

        Entity e0 = mScene->CreateEntity("first");

        MeshComponent& mesh = e0.AddComponent<MeshComponent>();
        MaterialComponent& mat = e0.AddComponent<MaterialComponent>();
        TransformComponent& trans = e0.AddComponent<TransformComponent>();

        class CubeRotater : public NativeScript {
        protected:
            void OnUpdate(float ts) override {
                TransformComponent& trans = GetComponent<TransformComponent>();
                trans.mRotation.y += ts * 0.2;
            }
        };

        e0.AddScript<CubeRotater>();

        mesh.mMesh = MeshFactory::GetQuadAsset();
        mat.mMaterial = matAsset;
        trans.mScale = glm::vec3(1.0f, 1.0f, 1.0f);
        trans.mRotation = glm::vec3(0.0f, 1.0f, 0.0f * 3.1415f / 4.0f);
        trans.mTranslation = glm::vec3(0.0f, 0.0f, -2);

        Entity c = mScene->CreateEntity("camera");

        CameraComponent& cam = c.AddComponent<CameraComponent>();
        TransformComponent& camTrans = c.AddComponent<TransformComponent>();

        class CameraController : public NativeScript {
        public:
            bool OnWindowResize(WindowResizeEvent* e) override {
                CameraComponent& camComp = GetComponent<CameraComponent>();

                float width = (float)e->mWidth;
                float height = (float)e->mHeight;

                camComp.mCamera.SetViewport(width, height);
                camComp.mCamera.SetAspect(width / height);

                return false;
            }

            bool OnMouseMoved(MouseMovedEvent* e) override {
                TransformComponent& trans = GetComponent<TransformComponent>();
               
                trans.mRotation.y += (float)e->mDeltaX * mSensitivity;
                trans.mRotation.x += (float)e->mDeltaY * -mSensitivity;

                return false;
            }


        protected:
            void OnCreate() override {
                AddEvent(EventType::WindowResize);
                AddEvent(EventType::MouseMoved);

                mForward = Input::GetScanCode(GM_KEY_W);
                mBack = Input::GetScanCode(GM_KEY_S);
                mRight = Input::GetScanCode(GM_KEY_D);
                mLeft = Input::GetScanCode(GM_KEY_A);
            }

            void OnDestroy() override {
                GM_LOG_DEBUG("CameraController destroy");
            }

            void OnUpdate(float ts) override {
                TransformComponent& trans = GetComponent<TransformComponent>();
                Camera& camera = GetComponent<CameraComponent>().mCamera;
                
                glm::mat4 rot(1.0f);
                rot = glm::rotate(rot, trans.mRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
                rot = glm::rotate(rot, trans.mRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));

                glm::vec3 forward = rot[2];
                glm::vec3 right = rot[0];

                glm::vec3 adj = glm::vec3(mSpeed, 0.0f, -mSpeed) * ts;
                forward *= adj;
                right *= adj;


                if (Input::IsKeyPressed(mForward))
                    trans.mTranslation += forward;
                else if (Input::IsKeyPressed(mBack))
                    trans.mTranslation -= forward;

                if (Input::IsKeyPressed(mRight))
                    trans.mTranslation += right;
                else if (Input::IsKeyPressed(mLeft))
                    trans.mTranslation -= right;

                camera.SetView(rot * glm::translate(glm::mat4(1.0f), -trans.mTranslation));

            }
        private:
            uint32_t mForward;
            uint32_t mBack;
            uint32_t mRight;
            uint32_t mLeft;

            float mSpeed = 1.5f;
            float mSensitivity = 0.001f;

        };

        c.AddScript<CameraController>();

        cam.mPrimary = true;
        cam.mCamera.SetPerspective(70.0f, (float)windowSpec.Width / windowSpec.Height, 0.001f, 100.0f);
        cam.mCamera.SetViewport(windowSpec.Width, windowSpec.Height);

        camTrans.mScale = glm::vec3(1.0f);
        camTrans.mRotation = glm::vec3(0.0f);
        camTrans.mTranslation = glm::vec3(0.0f, 0.0f, -0.5f);
    }

    void OnUpdate(float ts) override {
        mScene->OnUpdate(ts);
        mTime += ts;

        if (mTime >= 1.0f) {
            GM_LOG_INFO("[TestApp] FPS: {}", mFps);
            mFps = 0;
            mTime -= 1.0f;
        }
    }

    void OnRender() override {
        mScene->OnRender();
        mFps++;
    }

    void OnShutdown() override {
        delete mScene;
    }

    bool OnKeyPressed(KeyPressedEvent* e) override {
        const char* desc = Input::GetKeyInfo(e->mKey)->mString;
        GM_LOG_DEBUG("Key: {:02x} {}", e->mKey, desc);
        return false;
    }

    bool OnKeyReleased(KeyReleasedEvent* e) override {
        return false;
    }

    bool OnButtonPressed(ButtonPressedEvent* e) override {
        const char* desc = Input::GetKeyInfo(e->mButton)->mString;
        GM_LOG_DEBUG("Button: {:02x} {}", e->mButton, desc);
        return false;
    }

    bool OnButtonReleased(ButtonReleasedEvent* e) override {
        return false;
    }

    bool OnMouseMoved(MouseMovedEvent* e) override {
        return false;
    }

private:
    Scene* mScene;

    float mTime;
    uint16_t mFps;
};

int main() {
    GM_LOG_INFO("Start...");
    ApplicationSpec appSpec;

    appSpec.mName = "sa";
 
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

    ContextSpec cSpec;
    cSpec.applicationName = "TestApp";

    Context::Init(cSpec);

    Device* device = Context::CreateDevice(&window);

    SwapchainSpec scSpec;
    scSpec.mWindow = &window;
    scSpec.mPreferredPresentModes.push_back(VK_PRESENT_MODE_IMMEDIATE_KHR);
    scSpec.mDevice = device;

    AssetManager::Init(device);
    Swapchain* swapchain = Swapchain::CreateNew(scSpec);
    StagingManager::AllocateCommonStagingBuffer(device, std::this_thread::get_id(), 50000000, false);

    StagingBuffer* staging = StagingManager::GetCommonStagingBuffer();
    staging->Begin();

    {
        AssetHandle vertHandle = AssetManager::AddAsset(new Shader::Source("build/res/shader.vert", false, ShaderStage::Vertex), false);
        AssetHandle fragHandle = AssetManager::AddAsset(new Shader::Source("build/res/shader.frag", false, ShaderStage::Fragment), false);

        Shader shader(device);
        shader.AddModule(vertHandle, ShaderStage::Vertex);
        shader.AddModule(fragHandle, ShaderStage::Fragment);
        shader.Compile();

        Texture2D tex(device, 100, 100, VK_FORMAT_R32G32B32A32_SFLOAT);
        
        glm::vec4 colors[100 * 100];

        for (uint32_t y = 0; y < 100; y++) {
            for (uint32_t x = 0; x < 100; x++) {
                colors[y * 100 + x] = glm::vec4((float)x / 100.0f, 0, (float)y / 100.0f, 1);
            }
        }

        memcpy(staging->AllocateImage(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &tex), colors, sizeof(colors));

        AssetHandle sheetHandle = AssetManager::AddAsset(new Texture2D(device, "build/res/sheet.png"), false);
        AssetHandle texHandle = AssetManager::AddMemoryAsset(&tex, false);
        //AssetManager::AddAsset(new Mesh("res/mesh.obj"), false);
        AssetHandle planeHandle = AssetManager::AddMemoryAsset(Mesh::GeneratePlane(device));
        AssetHandle quadHandle = AssetManager::AddMemoryAsset(Mesh::GenerateQuad(device));

        BasicRenderpass pass(swapchain, device);

        PipelineLayout pipelineLayout(device, shader.GetDescriptorSetLayout(0), shader.GetPushConstants());

        GraphicsPipelineInfo gInfo;

        gInfo.mWidth = spec.Width;
        gInfo.mHeight = spec.Height;
        gInfo.mVertexInputBindings.push_back({ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        gInfo.mVertexInputAttributes = shader.GetVertexInputLayout({ {0, {0, 1, 2 } } });
        gInfo.mPipelineLayout = &pipelineLayout;
        gInfo.mRenderpass = &pass;
        gInfo.mShader = &shader;
        
        GraphicsPipeline gPipeline(device, gInfo);

        DescriptorPool mPool(device, 10);

        DescriptorSet set = mPool.AllocateDescriptorSet(shader.GetDescriptorSetLayout(0));
        VkDescriptorSet setHandle = set.GetHandle();

        Buffer uniform(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, set.GetLayout()->GetUniformBufferSize(0));
        Buffer uniform2(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, set.GetLayout()->GetUniformBufferSize(2));

        glm::mat4 matrix(1.0f);
        glm::vec4 color(1, 1, 1, 1);

        struct {
            glm::mat4 Model;
            glm::mat4 View;
            glm::mat4 Projection;
        } mvp;

        mvp.Model = glm::rotate(glm::mat4(1.0f), -1.5f, glm::vec3(1, 0, 0));
        mvp.View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -4));
        mvp.Projection = glm::perspective(70.0f, 16.0f / 9.0f, 0.0001f, 100.0f);

        memcpy(staging->Allocate(sizeof(color), &uniform), &color, sizeof(color));
        memcpy(staging->Allocate(sizeof(mvp), &uniform2), &mvp, sizeof(mvp));

        BasicSampler sampler(device);

        VkDescriptorBufferInfo bInfo;

        bInfo.buffer = uniform.GetHandle();
        bInfo.offset = 0;
        bInfo.range = sizeof(color);

        VkDescriptorBufferInfo bInfo2;

        bInfo2.buffer = uniform2.GetHandle();
        bInfo2.offset = 0;
        bInfo2.range = sizeof(matrix);

        VkWriteDescriptorSet wSet[3];

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

        wSet[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wSet[2].pNext = nullptr;
        wSet[2].dstSet = setHandle;
        wSet[2].dstBinding = 2;
        wSet[2].dstArrayElement = 0;
        wSet[2].descriptorCount = 1;
        wSet[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        wSet[2].pImageInfo = nullptr;
        wSet[2].pBufferInfo = &bInfo2;
        wSet[2].pTexelBufferView = nullptr;

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
        
        vkUpdateDescriptorSets(device->GetHandle(), 3, wSet, 0, nullptr);

        StagingManager::SubmitStagingBuffer(staging, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);

        while (!window.ShouldClose()) {
            EventManager::ProcessEvents(&window);
            auto start = std::chrono::high_resolution_clock::now();
            bool hasImage = swapchain->Begin();
            if (hasImage) {
                CommandBuffer* cmd = swapchain->GetRenderCommandBuffer();
                cmd->Wait();
                cmd->Begin(true);

                VkCommandBuffer handle = cmd->GetHandle();

                vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, gPipeline.GetHandle());

                pass.Begin(cmd);

                Mesh* mesh = AssetManager::GetAsset<Mesh>(quadHandle);

                VkDeviceSize offset = 0;
                VkBuffer vboHandle = mesh->GetVBOHandle();
                VkBuffer iboHandle = mesh->GetIBOHandle();
                
                vkCmdBindVertexBuffers(handle, 0, 1, &vboHandle, &offset);
                vkCmdBindIndexBuffer(handle, iboHandle, 0, mesh->GetIndexType());
                vkCmdBindDescriptorSets(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.GetHandle(), 0, 1, &setHandle, 0, 0);

                vkCmdDrawIndexed(handle, mesh->GetIndexCount(), 1, 0, 0, 0);

                pass.End(cmd);

                swapchain->Present();
            }

        }

        device->WaitQueueIdle();
    }

    AssetManager::Shutdown();
    StagingManager::Shutdown();
    Swapchain::Shutdown();
    Context::Shutdown();

    return 0;
}
