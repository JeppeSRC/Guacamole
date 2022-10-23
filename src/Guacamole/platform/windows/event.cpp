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

#include <Guacamole/core/event.h>

#include <Guacamole/core/Window.h>

namespace Guacamole {

Window* EventManager::mWindow = nullptr;

void EventManager::Init(const Window* window) {
    mWindow = const_cast<Window*>(window); // Dirty solution for the moment
}



void EventManager::ProcessEvents(Window* window) {
    MSG msg;

    while (PeekMessage(&msg, window->GetHWND(), 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void EventManager::Shutdown() {

}

LRESULT EventManager::WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {

    switch (msg) {
        case WM_CLOSE: {
            mWindow->mShouldClose = true;
            break;
        }

        case WM_KEYDOWN: {
            KeyPressedEvent evnt((uint32_t)w);

            DispatchEvent(&evnt);
            break;
        }

        case WM_KEYUP: {
            KeyReleasedEvent evnt((uint32_t)w);

            DispatchEvent(&evnt);
            break;
        }

        
    }


    return DefWindowProc(hwnd, msg, w, l);
}

}