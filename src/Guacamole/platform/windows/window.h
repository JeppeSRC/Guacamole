/*
MIT License

Copyright (c) 2025 Jesper

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

#include <Guacamole/core/video/window.h>

#include <Windows.h>

namespace Guacamole {

class WindowWindows : public Window {
public:
    WindowWindows(const WindowSpec& spec);
    ~WindowWindows();

    void CaptureInput() override;
    void ReleaseInput() override;
    void SetTitle(const char* title) override;

    void ProcessEvents() override;

private:
    HWND mHWND;

    static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
    static void CheckButton(uint16_t buttonFlags, uint16_t down, uint16_t up, uint32_t keyCode);
    static WindowWindows* sWindow;
    static uint32_t sLastMouseX;
    static uint32_t sLastMouseY;

public:
    HWND GetHWND() const { return mHWND; }
};
}