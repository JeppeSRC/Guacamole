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

Window::Window(WindowSpec spec) : mSpec(spec) {
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

    xcb_create_window(mConnection, 0, mWindow, screen->root, 0, 0, spec.Width, spec.Height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, 0 ,0);

    xcb_generic_error_t* error;
    xcb_randr_get_monitors_cookie_t monCoc = xcb_randr_get_monitors(mConnection, mWindow, 0);
    xcb_randr_get_monitors_reply_t* monRep = xcb_randr_get_monitors_reply(mConnection, monCoc, &error);
    xcb_randr_monitor_info_iterator_t monItr = xcb_randr_get_monitors_monitors_iterator(monRep);

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

    free(monRep);

    xcb_map_window(mConnection, mWindow);
    xcb_flush(mConnection);
}

Window::~Window() {
    xcb_destroy_window(mConnection, mWindow);
    xcb_disconnect(mConnection);
}

}