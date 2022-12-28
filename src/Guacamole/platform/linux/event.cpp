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

#include <Guacamole/core/video/event.h>
#include <Guacamole/core/video/window.h>
#include <Guacamole/core/input.h>

#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>

namespace Guacamole {

xkb_context* EventManager::mXkbContext;
int32_t EventManager::mKeyboardID;
xkb_keymap* EventManager::mKeymap;
xkb_state* EventManager::mState;

void EventManager::Init(Window* window) {
    mWindow = window;

    xcb_connection_t* conn = window->GetXCBConnection();

    mXkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    GM_VERIFY(mXkbContext);

    GM_VERIFY(xkb_x11_setup_xkb_extension(conn, 1, 0, XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, 0, 0, 0, 0) == 1);

    mKeyboardID = xkb_x11_get_core_keyboard_device_id(conn);
    GM_VERIFY(mKeyboardID != -1);

    mKeymap = xkb_x11_keymap_new_from_device(mXkbContext, conn, mKeyboardID, XKB_KEYMAP_COMPILE_NO_FLAGS);
    GM_VERIFY(mKeymap);

    mState = xkb_x11_state_new_from_device(mKeymap, conn, mKeyboardID);
    GM_VERIFY(mState);
}

//https://www.x.org/releases/current/doc/xproto/x11protocol.html#keysym_encoding

void EventManager::ProcessEvents(Window* window) {
    xcb_connection_t* conn = window->GetXCBConnection();
    xcb_generic_event_t* e = nullptr;

    while ((e = xcb_poll_for_event(conn))) {
        switch (e->response_type & 0x7F) {
            case XCB_KEY_PRESS: {
                if (!Input::mInputCapture) break;
                xcb_key_press_event_t* pressed = (xcb_key_press_event_t*)e;
                
                KeyPressedEvent evnt(pressed->detail);
                
                Input::OnKey(pressed->detail, true);
                DispatchEvent(&evnt);

                break;
            }

            case XCB_KEY_RELEASE: {
                if (!Input::mInputCapture) break;
                xcb_key_release_event_t* released = (xcb_key_release_event_t*)e;
                
                KeyReleasedEvent evnt(released->detail);
                
                Input::OnKey(released->detail, false);
                DispatchEvent(&evnt);

                break;
            }

            case XCB_BUTTON_PRESS: {
                xcb_button_press_event_t* pressed = (xcb_button_press_event_t*)e;
                
                ButtonPressedEvent evnt(pressed->detail  | GM_BUTTON_PREFIX);

                if (!Input::mInputCapture) {
                    if (evnt.mButton != GM_BUTTON_Left) break;

                    Input::CaptureInput();
                }

                Input::OnKey(pressed->detail | GM_BUTTON_PREFIX, true);
                DispatchEvent(&evnt);

                break;
            }

            case XCB_BUTTON_RELEASE: {
                if (!Input::mInputCapture) break;
                xcb_button_release_event_t* released = (xcb_button_release_event_t*)e;

                ButtonPressedEvent evnt(released->detail | GM_BUTTON_PREFIX);

                Input::OnKey(released->detail | GM_BUTTON_PREFIX, false);
                DispatchEvent(&evnt);

                break;
            }

            case XCB_MOTION_NOTIFY: {
                if (!Input::mInputCapture) break;
                xcb_motion_notify_event_t* motion = (xcb_motion_notify_event_t*)e;
                
                uint32_t x = motion->event_x;
                uint32_t y =  motion->event_y;

                uint32_t xMiddle = window->GetWidth() / 2;
                uint32_t yMiddle = window->GetHeight() / 2;
                
                if (x == xMiddle && y == yMiddle) break;

                int32_t dx = x - xMiddle;
                int32_t dy = y - yMiddle;

                MouseMovedEvent evnt(dx, dy);

                DispatchEvent(&evnt);

                xcb_connection_t* conn = window->GetXCBConnection();
                xcb_window_t win = window->GetXCBWindow();

                xcb_void_cookie_t cock = xcb_warp_pointer_checked(conn, win, win, 0, 0, window->GetWidth(), window->GetHeight(), xMiddle, yMiddle);

                xcb_generic_error_t* err = xcb_request_check(conn, cock);

                if (err) {
                    GM_LOG_CRITICAL("[EventManager] Error (0x{:04x}) calling xcb_warp_pointer_checked()", err->error_code);
                    free(err);
                }


                break;
            }

            case XCB_FOCUS_OUT: {
                Input::ReleaseInput();
                break;
            }

            case XCB_CLIENT_MESSAGE: {
                xcb_client_message_event_t* message = (xcb_client_message_event_t*)e;

                if (message->data.data32[0] == window->mWindowCloseAtom) {
                    window->mShouldClose = true;
                }

                break;
            }

            case XCB_CONFIGURE_NOTIFY: {
                xcb_configure_notify_event_t* notify = (xcb_configure_notify_event_t*)e;
                
                if (notify->width != window->mSpec.Width || notify->height != window->mSpec.Height) {
                    WindowResizeEvent evnt(notify->width, notify->height);

                    window->mSpec.Width = notify->width;
                    window->mSpec.Height = notify->height;

                    GM_LOG_DEBUG("[Window] Resize {}x{}", notify->width, notify->height);

                    DispatchEvent(&evnt);
                }
                
                

                break;
            }

        }

        free(e);
    }
}

void EventManager::Shutdown() {
    xkb_state_unref(mState);
    xkb_keymap_unref(mKeymap);
    xkb_context_unref(mXkbContext);
}


}