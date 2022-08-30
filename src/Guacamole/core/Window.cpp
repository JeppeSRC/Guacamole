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
#include "Window.h"

namespace Guacamole {

Window::Window(WindowSpec spec) : mSpec(spec) {
    int32_t count = 0;
    mMonitors = glfwGetMonitors(&count);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    mWindowHandle = glfwCreateWindow(mSpec.Width, mSpec.Height, mSpec.Title.c_str(), nullptr, nullptr);

    GM_ASSERT(mWindowHandle);

    if (mWindowHandle == nullptr) {
        GM_LOG_CRITICAL("Failed to create window");
        return;
    }

    GM_LOG_DEBUG("Window \"{0}\" created: {1}x{2}", spec.Title.c_str(), spec.Width, spec.Height);
}

Window::~Window() {
    glfwDestroyWindow(mWindowHandle);
}

}