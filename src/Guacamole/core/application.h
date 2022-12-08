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

#pragma once

#include <Guacamole.h>

#include <Guacamole/vulkan/swapchain.h>
#include <Guacamole/core/video/window.h>
#include <Guacamole/core/video/event.h>
#include <Guacamole/vulkan/device.h>

namespace Guacamole {

struct ApplicationSpec {
    std::string mName;
};

struct AppInitSpec {
    std::string appName;
    uint32_t deviceIndex;
};

class Application {
public:
    virtual ~Application();

    void Run();

    inline Window* GetWindow() const { return mWindow; }
    inline Swapchain* GetSwapchain() const { return mSwapchain; }
    inline Device* GetDevice() const { return mMainDevice; }

protected:
    Application(ApplicationSpec& spec);

    void Init(const WindowSpec& windowSpec, const AppInitSpec& appSpec);
    virtual void OnInit() = 0;
    virtual void OnUpdate(float ts) = 0;
    virtual void OnRender() = 0;
    virtual void OnShutdown() = 0;

    virtual bool OnKeyPressed(KeyPressedEvent* e) = 0;
    virtual bool OnKeyReleased(KeyReleasedEvent* e) = 0;
    virtual bool OnButtonPressed(ButtonPressedEvent* e) = 0;
    virtual bool OnButtonReleased(ButtonReleasedEvent* e) = 0;
    virtual bool OnMouseMoved(MouseMovedEvent* e) = 0;

    bool OnEvent(Event* e);

protected:
    ApplicationSpec mSpec;
    Window* mWindow;
    Swapchain* mSwapchain;
    Device* mMainDevice;
};

}