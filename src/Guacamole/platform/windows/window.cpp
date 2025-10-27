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

#include <Guacamole/core/video/window.h>

#include <Guacamole/core/video/event.h>

namespace Guacamole {

Window::Window(WindowSpec spec) : mSpec(spec), mShouldClose(true) {
    WNDCLASSEX wnd = {};

    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.hCursor = LoadCursor(0, IDC_ARROW);
    wnd.hIcon = LoadIcon(0, IDI_WINLOGO);
    wnd.lpfnWndProc = EventManager::WndProc;
    wnd.lpszClassName = L"Guacamole";
    wnd.style = CS_VREDRAW | CS_HREDRAW;

    GM_VERIFY(RegisterClassEx(&wnd));

    RECT r = { 0, 0, (LONG)spec.Width, (LONG)spec.Height };

    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);


    wchar_t title[256];
    mbstowcs(title, spec.Title.c_str(), 256);

    mHWND = CreateWindow(L"Guacamole", title, WS_OVERLAPPEDWINDOW, 0, 0, r.right - r.left, r.bottom - r.top, 0, 0, 0, 0);

    GM_VERIFY(mHWND);

    ShowWindow(mHWND, SW_SHOWNORMAL);

    mShouldClose = false;

    EventManager::Init(this);
}

Window::~Window() {
    ShowWindow(mHWND, SW_HIDE);
    DestroyWindow(mHWND);
    UnregisterClass(L"Guacamole", 0);
}
}