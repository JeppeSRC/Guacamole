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

#include <Guacamole/vulkan/swapchain.h>
#include <Guacamole/vulkan/context.h>
#include <Guacamole/asset/assetmanager.h>
#include <Guacamole/vulkan/commandpoolmanager.h>
#include <Guacamole/core/event.h>

#include <chrono>

namespace Guacamole {

Application::~Application() {

}

void Application::Run() {
    OnInit();

    GM_ASSERT(mWindow);

    auto last = std::chrono::high_resolution_clock::now();

    while (!mWindow->ShouldClose()) {

        EventManager::ProcessEvents(mWindow);

        auto now = std::chrono::high_resolution_clock::now();
        int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
        float delta = (float)duration / 1000.0f;

        OnUpdate(delta);

        OnRender();
    }

    OnShutdown();

    AssetManager::Shutdown();
    Swapchain::Shutdown();
    CommandPoolManager::Shutdown();
    Context::Shutdown();
    delete mWindow;
}

Application::Application(ApplicationSpec& spec) : mSpec(spec), mWindow(nullptr) {

}

void Application::Init(const WindowSpec& windowSpec) {
    mWindow = new Window(windowSpec);

    Context::Init(mWindow);
    Swapchain::Init(mWindow);
    CommandPoolManager::AllocateCopyCommandBuffers(std::this_thread::get_id(), 2);
    AssetManager::Init();

    EventManager::AddListener(EventType::KeyPressed, this, &Application::OnEvent);
    EventManager::AddListener(EventType::KeyReleased, this, &Application::OnEvent);
    EventManager::AddListener(EventType::ButtonPressed, this, &Application::OnEvent);
    EventManager::AddListener(EventType::ButtonReleased, this, &Application::OnEvent);
    EventManager::AddListener(EventType::MouseMoved, this, &Application::OnEvent);
}

bool Application::OnEvent(Event* e) {
    switch (e->GetType()) {
        case EventType::KeyPressed:
            return OnKeyPressed((KeyPressedEvent*)e);
            break;
        case EventType::KeyReleased:
            return OnKeyReleased((KeyReleasedEvent*)e);
            break;
        case EventType::ButtonPressed:
            return OnButtonPressed((ButtonPressedEvent*)e);
            break;
        case EventType::ButtonReleased:
            return OnButtonReleased((ButtonReleasedEvent*)e);
            break;
        case EventType::MouseMoved:
            return OnMouseMoved((MouseMovedEvent*)e);
            break;
    }

    return false;
}


}