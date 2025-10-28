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

#if defined(GM_WINDOW_XCB)

#include <xcb/randr.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>

namespace Guacamole {

class WindowXCB : public Window {
public:
    WindowXCB(const WindowSpec& spec);
    ~WindowXCB();

    void CaptureInput() override;
    void ReleaseInput() override;

    void ProcessEvents() override;

private:
    xcb_connection_t* mConnection;
    xcb_window_t mWindow;
    xcb_visualid_t mVisualID;
    xcb_atom_t mWindowCloseAtom;

    xkb_context* mXkbContext;
    int32_t mKeyboardID;
    xkb_keymap* mKeymap;
    xkb_state* mState;

    void InitXKB();

public:
    xcb_connection_t* GetXCBConnection() const { return mConnection; }
    xcb_window_t GetXCBWindow() const { return mWindow; }
    xcb_visualid_t GetVisualID() const { return mVisualID; }
    xkb_state* GetXKBState() const { return mState; }

};

}
#endif