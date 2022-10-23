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

#include <Guacamole/core/Window.h>
#include <Guacamole/core/monitor.h>

#include <Guacamole/core/event.h>

namespace Guacamole {

const char* GetAtomName(xcb_connection_t* conn, xcb_atom_t atom) {
    xcb_generic_error_t* error = nullptr;
    xcb_get_atom_name_cookie_t cookie = xcb_get_atom_name(conn, atom);
    xcb_get_atom_name_reply_t* reply = xcb_get_atom_name_reply(conn, cookie, &error);
    
    if (error) {
        GM_LOG_WARNING("Faild to get 'xcb_get_atom_name_reply_t*' error: {}", error->error_code);
        return "GetAtomNameError";
    }
    const char* str = xcb_get_atom_name_name(reply);

    free(reply);
    return str;
}

Window::Window(WindowSpec spec) : mSpec(spec), mShouldClose(true) {
    int32_t screenNum;
    mConnection = xcb_connect(nullptr, &screenNum);

    GM_VERIFY(xcb_connection_has_error(mConnection) <= 0);

    xcb_screen_t* screen;
    xcb_screen_iterator_t itr = xcb_setup_roots_iterator(xcb_get_setup(mConnection));

    for (; itr.rem; xcb_screen_next(&itr)) {
        screen = itr.data;
        if (screenNum-- == 0) break;
    }

    mWindow = xcb_generate_id(mConnection);
    mVisualID = screen->root_visual;

    uint32_t mask = XCB_CW_EVENT_MASK;
    uint32_t events = XCB_EVENT_MASK_BUTTON_PRESS   | XCB_EVENT_MASK_BUTTON_RELEASE |
                      XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE |
                      XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_FOCUS_CHANGE |
                      XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(mConnection, 0, mWindow, screen->root, 0, 0, spec.Width, spec.Height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask ,&events);

    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom(mConnection, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t* atomReply = xcb_intern_atom_reply(mConnection, atomCookie, nullptr);
    xcb_intern_atom_cookie_t propCookie = xcb_intern_atom(mConnection, 0, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* propReply = xcb_intern_atom_reply(mConnection, propCookie, nullptr);

    mWindowCloseAtom = atomReply->atom;

    xcb_change_property(mConnection, XCB_PROP_MODE_REPLACE, mWindow, propReply->atom, 4, 32, 1, &mWindowCloseAtom);

    xcb_randr_get_monitors_cookie_t monCookie = xcb_randr_get_monitors(mConnection, mWindow, 0);
    xcb_randr_get_monitors_reply_t* monReply = xcb_randr_get_monitors_reply(mConnection, monCookie, nullptr);
    xcb_randr_monitor_info_iterator_t monItr = xcb_randr_get_monitors_monitors_iterator(monReply);

    for (;monItr.rem; xcb_randr_monitor_info_next(&monItr)) {
        xcb_randr_monitor_info_t* info = monItr.data;

        Monitor mon;

        mon.mWidth = info->width;
        mon.mHeight = info->height;
        mon.mX = info->x;
        mon.mY = info->y;
        mon.mPrimary = info->primary;

        Monitor::Add(mon);
    }

    free(monReply);
    xcb_map_window(mConnection, mWindow);
    xcb_flush(mConnection);

    mShouldClose = false;

    EventManager::Init(this);
}

Window::~Window() {
    EventManager::Shutdown();
    xcb_destroy_window(mConnection, mWindow);
    xcb_disconnect(mConnection);
}

}