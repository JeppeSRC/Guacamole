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

#include "application.h"

#include <Guacamole/vulkan/context.h>
#include <Guacamole/asset/assetmanager.h>
#include <Guacamole/core/video/event.h>
#include <Guacamole/vulkan/buffer/stagingbuffer.h>
#include <Guacamole/renderer/meshfactory.h>
#include <Guacamole/core/input.h>

#include <chrono>

namespace Guacamole {

Application::~Application() {

}

void Application::Run() {
    OnInit();

    GM_ASSERT(mWindow);

    auto last = std::chrono::high_resolution_clock::now();

    while (!mWindow->ShouldClose()) {
        auto now = std::chrono::high_resolution_clock::now();
        int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last).count();
        float delta = (float)duration / 1000000.0f;
        last = now;

        StagingBuffer* commonStaging = StagingManager::GetCommonStagingBuffer();
        commonStaging->Begin();

        mWindow->ProcessEvents();

        bool shouldRender = mSwapchain->Begin();

        OnUpdate(delta);

        if (shouldRender) {
            OnRender();

            StagingManager::SubmitStagingBuffer(commonStaging, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);

            mSwapchain->Present();
        }

    }

    mMainDevice->WaitQueueIdle();

    OnShutdown();

    Input::Shutdown();
    EventManager::Shutdown();
    StagingManager::Shutdown();
    MeshFactory::Shutdown();
    AssetManager::Shutdown();
    Swapchain::Shutdown();
    Context::Shutdown();
    delete mWindow;
}

Application::Application(ApplicationSpec& spec) : mSpec(spec), mWindow(nullptr) {

}

void Application::Init(const WindowSpec& windowSpec, const AppInitSpec& appSpec) {
    mWindow = Window::Create(windowSpec);

    ContextSpec cs;
    cs.applicationName = appSpec.appName;

    Context::Init(cs);

    if (appSpec.deviceIndex == ~0u) {
        mMainDevice = Context::CreateDevice(mWindow);
    } else {
        mMainDevice = Context::CreateDevice(appSpec.deviceIndex);
    }

    SwapchainSpec ss;
    ss.mWindow = mWindow;
    ss.mPreferredPresentModes.push_back(VK_PRESENT_MODE_IMMEDIATE_KHR);
    ss.mDevice = mMainDevice;

    mSwapchain = Swapchain::CreateNew(ss);
    AssetManager::Init(mMainDevice);
    StagingManager::AllocateCommonStagingBuffer(ss.mDevice, std::this_thread::get_id(), 50000000, true);
    MeshFactory::Init(ss.mDevice);
    Input::Init();
    
    EventManager::Init(mWindow);
    EventManager::AddListener(EventType::KeyPressed, this, &Application::OnEvent);
    EventManager::AddListener(EventType::KeyReleased, this, &Application::OnEvent);
    EventManager::AddListener(EventType::ButtonPressed, this, &Application::OnEvent);
    EventManager::AddListener(EventType::ButtonReleased, this, &Application::OnEvent);
    EventManager::AddListener(EventType::MouseMoved, this, &Application::OnEvent);

}

bool Application::OnEvent(Event* e) {
    switch (e->GetType()) {
        case EventType::KeyPressed: {
            KeyPressedEvent* evnt = (KeyPressedEvent*)e;

            // Hardcoded for now
            if (Input::GetScanCode(GM_KEY_Escape) == evnt->mKey) mWindow->ReleaseInput();

            return OnKeyPressed(evnt);
        }
        case EventType::KeyReleased:
            return OnKeyReleased((KeyReleasedEvent*)e);
        case EventType::ButtonPressed:
            return OnButtonPressed((ButtonPressedEvent*)e);
        case EventType::ButtonReleased:
            return OnButtonReleased((ButtonReleasedEvent*)e);
        case EventType::MouseMoved:
            return OnMouseMoved((MouseMovedEvent*)e);
        default:
            GM_ASSERT_MSG(false, "Ont implemented yet");
    }

    return false;
}


}