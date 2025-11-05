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


#include <Guacamole/platform/linux/window.h>
#include <Guacamole/core/video/monitor.h>
#include <Guacamole/core/video/event.h>
#include <Guacamole/core/input.h>

#include <xcb/xfixes.h>
#include "window.h"

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

WindowXCB::WindowXCB(const WindowSpec& spec) : Window(spec, Type::XCB) {
    int32_t screenNum;
    mConnection = xcb_connect(nullptr, &screenNum);

    GM_VERIFY(xcb_connection_has_error(mConnection) <= 0);

    xcb_screen_t* screen = nullptr;
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
                      XCB_EVENT_MASK_EXPOSURE       | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    xcb_create_window(mConnection, 0, mWindow, screen->root, 0, 0, spec.Width, spec.Height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask ,&events);

    xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom(mConnection, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t* atomReply = xcb_intern_atom_reply(mConnection, atomCookie, nullptr);
    xcb_intern_atom_cookie_t propCookie = xcb_intern_atom(mConnection, 0, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* propReply = xcb_intern_atom_reply(mConnection, propCookie, nullptr);
   

    mWindowCloseAtom = atomReply->atom;

    xcb_change_property(mConnection, XCB_PROP_MODE_REPLACE, mWindow, propReply->atom, XCB_ATOM_ATOM, 32, 1, &mWindowCloseAtom);
    
    free(atomReply);
    free(propReply);

    SetTitle(spec.Title.c_str());

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

    xcb_xfixes_query_version(mConnection, 4, 0);

    InitXKB();
}

void WindowXCB::CaptureInput() {
 if (mInputCapture) return;

    mInputCapture = true;

    xcb_void_cookie_t cookie = xcb_xfixes_hide_cursor_checked(mConnection, mWindow);
    xcb_generic_error_t* err = xcb_request_check(mConnection, cookie);

    if (err) {
        GM_LOG_CRITICAL("[Input] Error (0x{:04x}) calling xcb_xfixes_hide_cursor_checked()", err->error_code);
        free(err);
    }

    GM_LOG_DEBUG("[Input] Mouse captured!");
}

void WindowXCB::ReleaseInput() {
if (!mInputCapture) return;

    mInputCapture = false;

    xcb_void_cookie_t cookie = xcb_xfixes_show_cursor_checked(mConnection, mWindow);
    xcb_generic_error_t* err = xcb_request_check(mConnection, cookie);

    if (err) {
        GM_LOG_CRITICAL("[Input] Error (0x{:04x}) calling xcb_xfixes_show_cursor_checked()", err->error_code);
        free(err);
    }

    GM_LOG_DEBUG("[Input] Mouse released!");
}

void WindowXCB::SetTitle(const char *title) {
    xcb_intern_atom_cookie_t titleCookie = xcb_intern_atom(mConnection, 0, 7, "WM_NAME");
    xcb_intern_atom_reply_t* titleReply = xcb_intern_atom_reply(mConnection, titleCookie, nullptr);

    xcb_change_property(mConnection, XCB_PROP_MODE_REPLACE, mWindow, titleReply->atom, XCB_ATOM_STRING, 8, strlen(title), title);
    free(titleReply);
}
 // https://www.x.org/releases/current/doc/xproto/x11protocol.html#keysym_encoding

void WindowXCB::ProcessEvents() {
    xcb_generic_event_t* e = nullptr;

    while ((e = xcb_poll_for_event(mConnection))) {
        switch (e->response_type & 0x7F) {
            case XCB_KEY_PRESS: {
                if (!mInputCapture) break;
                xcb_key_press_event_t* pressed = (xcb_key_press_event_t*)e;
                
                KeyPressedEvent evnt(pressed->detail-8);
                
                Input::OnKey(evnt.mKey, true);
                EventManager::DispatchEvent(&evnt);
                
                break;
            }

            case XCB_KEY_RELEASE: {
                if (!mInputCapture) break;
                xcb_key_release_event_t* released = (xcb_key_release_event_t*)e;
                
                KeyReleasedEvent evnt(released->detail-8);
                
                Input::OnKey(evnt.mKey, false);
                EventManager::DispatchEvent(&evnt);

                break;
            }

            case XCB_BUTTON_PRESS: {
                xcb_button_press_event_t* pressed = (xcb_button_press_event_t*)e;
                
                ButtonPressedEvent evnt(pressed->detail  | GM_BUTTON_PREFIX);

                if (!mInputCapture) {
                    if (evnt.mButton != GM_BUTTON_Left) break;

                    CaptureInput();
                }

                Input::OnKey(pressed->detail | GM_BUTTON_PREFIX, true);
                EventManager::DispatchEvent(&evnt);

                break;
            }

            case XCB_BUTTON_RELEASE: {
                if (!mInputCapture) break;
                xcb_button_release_event_t* released = (xcb_button_release_event_t*)e;

                ButtonPressedEvent evnt(released->detail | GM_BUTTON_PREFIX);

                Input::OnKey(released->detail | GM_BUTTON_PREFIX, false);
                EventManager::DispatchEvent(&evnt);

                break;
            }

            case XCB_MOTION_NOTIFY: {
                if (!mInputCapture) break;
                xcb_motion_notify_event_t* motion = (xcb_motion_notify_event_t*)e;
                
                uint32_t x = motion->event_x;
                uint32_t y =  motion->event_y;

                uint32_t xMiddle = GetWidth() / 2;
                uint32_t yMiddle = GetHeight() / 2;
                
                if (x == xMiddle && y == yMiddle) break;

                int32_t dx = x - xMiddle;
                int32_t dy = y - yMiddle;
                
                MouseMovedEvent evnt(dx, dy);

                EventManager::DispatchEvent(&evnt);

                xcb_void_cookie_t cock = xcb_warp_pointer_checked(mConnection, mWindow, mWindow, 0, 0, GetWidth(), GetHeight(), xMiddle, yMiddle);
                xcb_generic_error_t* err = xcb_request_check(mConnection, cock);

                if (err) {
                    GM_LOG_CRITICAL("[EventManager] Error (0x{:04x}) calling xcb_warp_pointer_checked()", err->error_code);
                    free(err);
                }


                break;
            }

            case XCB_FOCUS_OUT: {
                ReleaseInput();
                break;
            }

            case XCB_CLIENT_MESSAGE: {
                xcb_client_message_event_t* message = (xcb_client_message_event_t*)e;

                if (message->data.data32[0] == mWindowCloseAtom) {
                    mShouldClose = true;
                }

                break;
            }

            case XCB_CONFIGURE_NOTIFY: {
                xcb_configure_notify_event_t* notify = (xcb_configure_notify_event_t*)e;
                
                if (notify->width != mSpec.Width || notify->height != mSpec.Height) {
                    WindowResizeEvent evnt(notify->width, notify->height);

                    mSpec.Width = notify->width;
                    mSpec.Height = notify->height;

                    GM_LOG_DEBUG("[Window] Resize {}x{}", notify->width, notify->height);

                    EventManager::DispatchEvent(&evnt);
                }
                
                

                break;
            }

        }

        free(e);
    }
}

WindowXCB::~WindowXCB() {
    xkb_state_unref(mState);
    xkb_keymap_unref(mKeymap);
    xkb_context_unref(mXkbContext);

    xcb_destroy_window(mConnection, mWindow);
    xcb_disconnect(mConnection);
}

void WindowXCB::InitXKB() {
    mXkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    GM_VERIFY(mXkbContext);

    GM_VERIFY(xkb_x11_setup_xkb_extension(mConnection, 1, 0, XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, 0, 0, 0, 0) == 1);

    mKeyboardID = xkb_x11_get_core_keyboard_device_id(mConnection);
    GM_VERIFY(mKeyboardID != -1);

    mKeymap = xkb_x11_keymap_new_from_device(mXkbContext, mConnection, mKeyboardID, XKB_KEYMAP_COMPILE_NO_FLAGS);
    GM_VERIFY(mKeymap);

    mState = xkb_x11_state_new_from_device(mKeymap, mConnection, mKeyboardID);
    GM_VERIFY(mState);
}

}