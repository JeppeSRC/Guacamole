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

#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>

#if defined(GM_WINDOW_XCB)

#include <xcb/randr.h>

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

#if defined(GM_WINDOW_WAYLAND)

struct wl_display;
struct wl_registry;
struct wl_surface;
struct wl_compositor;
struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;
struct wl_seat;
struct wl_keyboard;
struct wl_pointer;
struct wl_array;
struct zwp_locked_pointer_v1;
struct zwp_pointer_constraints_v1;
struct zwp_relative_pointer_manager_v1;
struct zwp_relative_pointer_v1;
struct zxdg_decoration_manager_v1;
struct zxdg_toplevel_decoration_v1;

namespace Guacamole {

class WindowWayland : public Window {
public:
    WindowWayland(const WindowSpec& spec);
    ~WindowWayland();

    void CaptureInput() override;
    void ReleaseInput() override;

    void ProcessEvents() override;

    inline wl_display* GetDisplay() const { return mDisplay; }
    inline wl_surface* GetSurface() const { return mSurface; }

    inline xkb_state* GetXKBState() { return mState; }

private:
    wl_display* mDisplay;
    wl_registry* mRegistry;
    wl_compositor* mCompositor;
    wl_surface* mSurface;
    xdg_wm_base* mXDGBase;
    xdg_surface* mXDGSurface;
    xdg_toplevel* mTopLevel;
    wl_seat* mSeat;
    wl_keyboard* mKeyboard;
    wl_pointer* mPointer;
    uint32_t mPointerLastSerial;

    zwp_locked_pointer_v1* mLockedPointer;
    zwp_pointer_constraints_v1* mPointerConstrains;
    zwp_relative_pointer_manager_v1* mRelativePointerManager;
    zwp_relative_pointer_v1* mRelativePointer;

    zxdg_decoration_manager_v1* mDecorationManager;
    zxdg_toplevel_decoration_v1* mDecorationTopLevel;

    xkb_context* mContext;
    xkb_state* mState;
    xkb_keymap* mKeymap;

    void InitXKB(const char* keymap);

public: // Wayland callbacks
    static void DisplayErrorEvent(void* data, wl_display* display, void* objectID, uint32_t code, const char* message);
    static void DispalyDeleteIDEvent(void* data, wl_display* display, uint32_t id);

    static void RegistryGlobalEvent(void* data, wl_registry* wl_registry, uint32_t name, const char* interface, uint32_t version);
    static void RegistryGlobalRemoveEvent(void* data, wl_registry* wl_registry, uint32_t name);

    static void XDGSurfaceConfigureEvent(void* data, struct xdg_surface* xdg_surface, uint32_t serial);

    static void ToplevelConfigure(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height, struct wl_array *states);
	static void ToplevelClose(void *data, struct xdg_toplevel *xdg_toplevel);
	static void ToplevelConfigureBounds(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height);
	static void ToplevelWMCapabilities(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities);

    static void SeatCapabilitiesEvent(void* data, wl_seat* seat, uint32_t capabilities);
    static void SeatNameEvent(void* data, wl_seat* seat, const char* name);
    
    static void KeyboardEnterEvent(void* data, struct wl_keyboard* wl_keyboard, uint32_t serial, struct wl_surface* surface, wl_array* keys);
    static void KeyboardLeaveEvent(void* data, struct wl_keyboard* wl_keyboard, uint32_t serial, struct wl_surface* surface);
    static void KeyboardKeyEvent(void* data, struct wl_keyboard* wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
    static void KeyboardModifiersEvent(void* data, struct wl_keyboard* wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
    static void KeyboardRepeatInfoEvent(void* data, struct wl_keyboard* wl_keyboard, int32_t rate, int32_t delay);
    static void KeyboardKeymapEvent(void* data, struct wl_keyboard* wl_keyboard, uint32_t format, int32_t fd, uint32_t size);
    
    static void PointerEnterEvent(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, int32_t surface_x, int32_t surface_y);
    static void PointerLeaveEvent(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface);
    static void PointerMotionEvent(void* data, wl_pointer* pointer, uint32_t time, int32_t surface_x, int32_t surface_y);
    static void PointerButtonEvent(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    static void PointerAxisEvent(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, int32_t value);
    static void PointerFrameEvent(void* data, wl_pointer* pointer);
    static void PointerAxisSourceEvent(void* data, wl_pointer* pointer, uint32_t axis_source);
    static void PointerAxisStopEvent(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis);
    static void PointerAxisDiscreteEvent(void* data, wl_pointer* pointer, uint32_t axis, int32_t discrete);
    static void PointerAxisValue120Event(void* data, wl_pointer* pointer, uint32_t axis, int32_t value120);
    static void PointerAxisRelativeDirectionEvent(void* data, wl_pointer* pointer, uint32_t axis, uint32_t direction);

    static void PointerLockedEvent(void* data, zwp_locked_pointer_v1* pointer);
    static void PointerUnlockedEvent(void* data, zwp_locked_pointer_v1* pointer);
    static void PointerRelativeMotionEvent(void* data, zwp_relative_pointer_v1* pointer, uint32_t utime_hi, uint32_t utime_lo, int32_t dx, int32_t dy, int32_t dx_unaccel, int32_t dy_unaccel);

    static void DecorationTopLevelConfigure(void* data, zxdg_toplevel_decoration_v1* toplevel, uint32_t mode);
};

}

#endif