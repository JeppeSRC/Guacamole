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
#include <Guacamole/scene/scripts/nativescript.h>
#include <Guacamole/scene/scripts/cameracontroller.h>

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

        AssetHandle texAsset = AssetManager::AddAsset(new Texture2D(mMainDevice, "build/res/sheet.png"), true);
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

        c.AddScript<Script::CameraController>();

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
            mTime = 0.0f;
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

    float mTime = 0.0f;
    uint16_t mFps;
};

int main() {
    GM_LOG_INFO("Start...");
    ApplicationSpec appSpec;

    appSpec.mName = "sa";

    TestApp app(appSpec);

    app.Run();
    return 0;
}
