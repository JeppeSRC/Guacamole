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


#if defined(GM_LINUX)

#include <xcb/randr.h>

#elif defined(GM_WINDOWS)

#include <Windows.h>

#endif

namespace Guacamole {

struct WindowSpec {
    uint32_t Width;
    uint32_t Height;
    uint32_t Refreshrate;

    bool Windowed;

    std::string Title;
};

class Window {
public:
    Window(WindowSpec spec);
    ~Window();

    inline const WindowSpec& GetSpec() const { return mSpec; }
    inline bool ShouldClose() const { return mShouldClose; }
    inline uint32_t GetWidth() const { return mSpec.Width; }
    inline uint32_t GetHeight() const { return mSpec.Height; }

private:
    WindowSpec mSpec;
    bool mShouldClose;


// Platform specific region
#if defined(GM_LINUX)
private:
    xcb_connection_t* mConnection;
    xcb_window_t mWindow;
    xcb_visualid_t mVisualID;
    xcb_atom_t mWindowCloseAtom;

public:
    xcb_connection_t* GetXCBConnection() const { return mConnection; }
    xcb_window_t GetXCBWindow() const { return mWindow; }
    xcb_visualid_t GetVisualID() const { return mVisualID; }

#elif defined(GM_WINDOWS)
private:
    HWND mHWND;

public:
    HWND GetHWND() const { return mHWND; }
#endif

friend class EventManager;
};


}