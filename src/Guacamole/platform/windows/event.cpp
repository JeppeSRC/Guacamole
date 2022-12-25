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

#include <Guacamole/core/video/event.h>

#include <Guacamole/core/video/Window.h>
#include <Guacamole/core/input.h>

namespace Guacamole {

Window* EventManager::mWindow = nullptr;

void EventManager::Init(const Window* window) {
    mWindow = const_cast<Window*>(window); // Dirty solution for the moment

    RAWINPUTDEVICE rid;

    rid.usUsagePage = 0x01;
    rid.usUsage = 0x06;
    rid.dwFlags = 0;
    rid.hwndTarget = mWindow->GetHWND();

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        DWORD error = GetLastError();

        GM_LOG_CRITICAL("Failed to register input devices: 0x{:0x08}", error);
        return;
    }
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

       /* case WM_KEYDOWN: {

            KeyPressedEvent evnt((uint32_t)w);
            
            DispatchEvent(&evnt);
            break;
        }

        case WM_KEYUP: {
            KeyReleasedEvent evnt((uint32_t)w);

            DispatchEvent(&evnt);
            break;
        }*/

        case WM_INPUT: {
            UINT size;

            GetRawInputData((HRAWINPUT)l, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
            uint8_t* data = new uint8_t[size];

            UINT retSize = GetRawInputData((HRAWINPUT)l, RID_INPUT, data, &size, sizeof(RAWINPUTHEADER));

            if (retSize != size) {
                GM_LOG_DEBUG("GetRawInputData doesn't return the correct size, {} should be {}", retSize, size);
            }

            RAWINPUT* raw = (RAWINPUT*)data;

            if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                RAWKEYBOARD& kb = raw->data.keyboard;

                if (kb.Flags & RI_KEY_E0)
                    kb.MakeCode |= 0xE000;
                else if (kb.Flags & RI_KEY_E1)
                    kb.MakeCode |= 0xE100;

                if (kb.MakeCode == 0xE02A) break; 

                kb.VKey = MapVirtualKey(kb.MakeCode, MAPVK_VSC_TO_VK_EX);
                kb.VKey |= (kb.MakeCode & 0xE100);
                
                //GM_LOG_DEBUG(u8"ScanCode 0x{:04x} KeyCode 0x{:04x}", kb.MakeCode, kb.VKey);

                if (kb.Flags & RI_KEY_BREAK) {
                    Input::OnKey(kb.MakeCode, false);

                    KeyReleasedEvent evnt((uint32_t)kb.MakeCode);

                    DispatchEvent(&evnt);
                } else if (!Input::IsKeyPressed(kb.MakeCode)) {
                    Input::OnKey(kb.MakeCode, true);
                    KeyPressedEvent evnt((uint32_t)kb.MakeCode);

                    DispatchEvent(&evnt);
                }

                delete[] data;
            }

            break;
        }


        
    }


    return DefWindowProc(hwnd, msg, w, l);
}

}