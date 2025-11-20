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

#include <Guacamole.h>

#include "window.h"

#include <Guacamole/core/input.h>

#include <wayland-client.h>
#include <xdg-shell.h>
#include <relative-pointer.h>
#include <pointer-constraints.h>
#include <xdg-decorations.h>

#include <sys/mman.h>
#include <unistd.h>
#include <linux/input-event-codes.h>

namespace Guacamole {

wl_display_listener sDisplayListener =  {
    .error = WindowWayland::DisplayErrorEvent,
    .delete_id = WindowWayland::DispalyDeleteIDEvent
};

wl_registry_listener sRegistryListener = {
    .global = WindowWayland::RegistryGlobalEvent,
    .global_remove = WindowWayland::RegistryGlobalRemoveEvent
};

xdg_surface_listener sXDGSurfaceListener = {
    .configure = WindowWayland::XDGSurfaceConfigureEvent
};

void xdg_pong(void* data, xdg_wm_base* xdgBase, uint32_t serial) {
    GM_LOG_DEBUG("[Wayland] Pong!!!!!");
    xdg_wm_base_pong(xdgBase, serial);
}

xdg_wm_base_listener sXDGBaseListener = {
    .ping = xdg_pong
};

wl_seat_listener sSeatListener = {
    .capabilities = WindowWayland::SeatCapabilitiesEvent,
    .name = WindowWayland::SeatNameEvent
};

wl_keyboard_listener sKeyboardListener = {
    .keymap = WindowWayland::KeyboardKeymapEvent,
    .enter = WindowWayland::KeyboardEnterEvent,
    .leave = WindowWayland::KeyboardLeaveEvent,
    .key = WindowWayland::KeyboardKeyEvent,
    .modifiers = WindowWayland::KeyboardModifiersEvent,
    .repeat_info = WindowWayland::KeyboardRepeatInfoEvent
};

wl_pointer_listener sPointerListener = {
    .enter = WindowWayland::PointerEnterEvent,
    .leave = WindowWayland::PointerLeaveEvent,
    .motion = WindowWayland::PointerMotionEvent,
    .button = WindowWayland::PointerButtonEvent,
    .axis = WindowWayland::PointerAxisEvent,
    .frame = WindowWayland::PointerFrameEvent,
    .axis_source = WindowWayland::PointerAxisSourceEvent,
    .axis_discrete = WindowWayland::PointerAxisDiscreteEvent,
    .axis_value120 = WindowWayland::PointerAxisValue120Event,
    .axis_relative_direction = WindowWayland::PointerAxisRelativeDirectionEvent
};

zwp_relative_pointer_v1_listener sRelativePointerListener = {
    .relative_motion = WindowWayland::PointerRelativeMotionEvent
};

zwp_locked_pointer_v1_listener sLockedPointerListener = {
    .locked = WindowWayland::PointerLockedEvent,
    .unlocked = WindowWayland::PointerUnlockedEvent
};

zxdg_toplevel_decoration_v1_listener sDecorationTopLevelListener = {
    .configure = WindowWayland::DecorationTopLevelConfigure
};

xdg_toplevel_listener sToplevelListener = {
    .configure = WindowWayland::ToplevelConfigure,
    .close = WindowWayland::ToplevelClose,
    .configure_bounds = WindowWayland::ToplevelConfigureBounds,
    .wm_capabilities = WindowWayland::ToplevelWMCapabilities
};

WindowWayland::WindowWayland(const WindowSpec& spec) : Window(spec, Type::Wayland), mLockedPointer(nullptr) {
    mDisplay = wl_display_connect(nullptr);
    mRegistry = wl_display_get_registry(mDisplay);
    
    wl_display_add_listener(mDisplay, &sDisplayListener, this);
    wl_registry_add_listener(mRegistry, &sRegistryListener, this);

    wl_display_roundtrip(mDisplay);
    
    mSurface = wl_compositor_create_surface(mCompositor);
    mXDGSurface = xdg_wm_base_get_xdg_surface(mXDGBase, mSurface);
    mTopLevel = xdg_surface_get_toplevel(mXDGSurface);
    
    xdg_wm_base_add_listener(mXDGBase, &sXDGBaseListener, this);
    xdg_surface_add_listener(mXDGSurface, &sXDGSurfaceListener, this);
    xdg_toplevel_add_listener(mTopLevel, &sToplevelListener, this);
    
    xdg_toplevel_set_title(mTopLevel, spec.Title.c_str());
    xdg_toplevel_set_app_id(mTopLevel, spec.Title.c_str());

    mDecorationTopLevel = zxdg_decoration_manager_v1_get_toplevel_decoration(mDecorationManager, mTopLevel);

    zxdg_toplevel_decoration_v1_add_listener(mDecorationTopLevel, &sDecorationTopLevelListener, this);

    zxdg_toplevel_decoration_v1_set_mode(mDecorationTopLevel, 2);
    
    wl_seat_add_listener(mSeat, &sSeatListener, this);

    wl_surface_commit(mSurface);
    wl_display_roundtrip(mDisplay);
    wl_display_roundtrip(mDisplay);

    mShouldClose = false;
}

WindowWayland::~WindowWayland() {
    if (mLockedPointer)
        zwp_locked_pointer_v1_destroy(mLockedPointer);

    zwp_relative_pointer_v1_destroy(mRelativePointer);
    zwp_relative_pointer_manager_v1_destroy(mRelativePointerManager);
    zwp_pointer_constraints_v1_destroy(mPointerConstrains);
    wl_keyboard_destroy(mKeyboard);
    wl_pointer_destroy(mPointer);
    wl_seat_destroy(mSeat);
    zxdg_toplevel_decoration_v1_destroy(mDecorationTopLevel);
    zxdg_decoration_manager_v1_destroy(mDecorationManager);
    xdg_toplevel_destroy(mTopLevel);
    xdg_surface_destroy(mXDGSurface);
    xdg_wm_base_destroy(mXDGBase);
    wl_surface_destroy(mSurface);
    wl_compositor_destroy(mCompositor);
    wl_display_disconnect(mDisplay);

    xkb_state_unref(mState);
    xkb_keymap_unref(mKeymap);
    xkb_context_unref(mContext);
}

void WindowWayland::CaptureInput() {
    GM_ASSERT(mInputCapture == false);
    mLockedPointer = zwp_pointer_constraints_v1_lock_pointer(mPointerConstrains, mSurface, mPointer, nullptr, ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_ONESHOT);

    zwp_locked_pointer_v1_add_listener(mLockedPointer, &sLockedPointerListener, this);
    wl_pointer_set_cursor(mPointer, mPointerLastSerial, 0, 0, 0); 
}


// TODO: reset cursor when input is released, atm the cursor needs to leave the surface to be restored.
void WindowWayland::ReleaseInput() {
    GM_ASSERT(mInputCapture == true);
    zwp_locked_pointer_v1_destroy(mLockedPointer);
    mLockedPointer = nullptr;
    mInputCapture = false;
}

void WindowWayland::SetTitle(const char* title) {
    xdg_toplevel_set_title(mTopLevel, title);
}

void WindowWayland::ProcessEvents() {
    wl_display_roundtrip(mDisplay);
    wl_surface_commit(mSurface);
}

void WindowWayland::InitXKB(const char* keymap) {
    mContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    GM_VERIFY(mContext);
 
    mKeymap = xkb_keymap_new_from_string(mContext, keymap, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    GM_VERIFY(mKeymap);

    mState = xkb_state_new(mKeymap);
    GM_VERIFY(mState);
}

void WindowWayland::DisplayErrorEvent(void *data, wl_display *display, void *objectID, uint32_t code, const char *message) {
    GM_LOG_CRITICAL("[Wayland] wl_display.error: Error={} Message={}", code, message);
}

void WindowWayland::DispalyDeleteIDEvent(void *data, wl_display *display, uint32_t id) {
    GM_LOG_CRITICAL("[Wayland] wl_display.delete_id: ID={}", id);
}

void WindowWayland::RegistryGlobalEvent(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    WindowWayland* window = (WindowWayland*)data;

    GM_LOG_DEBUG("[Wayland] wl_registry.global: Name={} Interface={} Version={}", name, interface, version);

    if (strcmp(interface, "wl_compositor") == 0) {
        window->mCompositor = (wl_compositor*)wl_registry_bind(registry, name, &wl_compositor_interface, version);
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        window->mXDGBase = (xdg_wm_base*)wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
    } else if (strcmp(interface, "wl_seat") == 0) {
        window->mSeat = (wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, version);
    } else if (strcmp(interface, "zwp_pointer_constraints_v1") == 0) {
        window->mPointerConstrains = (zwp_pointer_constraints_v1*)wl_registry_bind(registry, name, &zwp_pointer_constraints_v1_interface, version);
    } else if (strcmp(interface, "zwp_relative_pointer_manager_v1") == 0) {
        window->mRelativePointerManager = (zwp_relative_pointer_manager_v1*)wl_registry_bind(registry, name, &zwp_relative_pointer_manager_v1_interface, version);
    } else if (strcmp(interface, "zxdg_decoration_manager_v1") == 0) {
        window->mDecorationManager = (zxdg_decoration_manager_v1*)wl_registry_bind(registry, name, &zxdg_decoration_manager_v1_interface, version);
    } 
}

void WindowWayland::RegistryGlobalRemoveEvent(void* data, wl_registry* registry, uint32_t name) {
    GM_LOG_DEBUG("[Wayland] wl_registry.global_remove: Name={}", name);
}

void WindowWayland::XDGSurfaceConfigureEvent(void* data, xdg_surface* xdg_surface, uint32_t serial) {
    //GM_LOG_DEBUG("[Wayland] xdg_surface.configure");

    xdg_surface_ack_configure(xdg_surface, serial);
}

void WindowWayland::ToplevelConfigure(void *data, xdg_toplevel *xdg_toplevel, int32_t width, int32_t height, wl_array* states) {
    //GM_LOG_DEBUG("[Wayland] xdg_toplevel.configure: Width={} Height={}", width, height);

    if (width == 0 || height == 0) return;

    WindowWayland* window = (WindowWayland*)data;

    if ((uint16_t)width == window->GetWidth() && (uint16_t)height == window->GetHeight()) return;

    WindowResizeEvent evnt((uint16_t)width, (uint16_t)height);
    EventManager::DispatchEvent(&evnt);
}

void WindowWayland::ToplevelClose(void *data, xdg_toplevel *xdg_toplevel) {
    GM_LOG_DEBUG("[Wayland] xdg_toplevel.close");
    ((WindowWayland*)data)->mShouldClose = true;
}

void WindowWayland::ToplevelConfigureBounds(void *data, xdg_toplevel *xdg_toplevel, int32_t width, int32_t height) {
    //GM_LOG_DEBUG("[Wayland] xdg_toplevel.configure_bounds: Width={} Height={}", width, height);
}

void WindowWayland::ToplevelWMCapabilities(void *data, xdg_toplevel *xdg_toplevel, wl_array *capabilities) {
    //GM_LOG_DEBUG("[Wayland] xdg_toplevel.wm_capabilities");
}

void WindowWayland::SeatCapabilitiesEvent(void* data, wl_seat* seat, uint32_t capabilities) {
    GM_LOG_DEBUG("[Wayland] wl_seat.capabilities: Capabilities={:x}", capabilities);
    GM_VERIFY_MSG(capabilities >= 3, "No keyboard and mouse support");

    WindowWayland* window = (WindowWayland*)data;

    window->mKeyboard = wl_seat_get_keyboard(seat);
    window->mPointer = wl_seat_get_pointer(seat);
    window->mRelativePointer = zwp_relative_pointer_manager_v1_get_relative_pointer(window->mRelativePointerManager, window->mPointer);

    wl_keyboard_add_listener(window->mKeyboard, &sKeyboardListener, data);
    wl_pointer_add_listener(window->mPointer, &sPointerListener, data);
    zwp_relative_pointer_v1_add_listener(window->mRelativePointer, &sRelativePointerListener, data);
}

void WindowWayland::SeatNameEvent(void* data, wl_seat* seat, const char* name) {
    GM_LOG_DEBUG("[Wayland] wl_seat.name: Name={}", name);
}

void WindowWayland::KeyboardEnterEvent(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface, wl_array *keys) {
    GM_LOG_DEBUG("[Wayland] wl_keyboard.enter");
}

void WindowWayland::KeyboardLeaveEvent(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface) {
    GM_LOG_DEBUG("[Wayland] wl_keyboard.leave");
}

void WindowWayland::KeyboardKeyEvent(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
    //GM_LOG_DEBUG("[Wayland] wl_keyboard.key: Key=0x{:X} State={}", key, state);

    WindowWayland* window = (WindowWayland*)data;
    
    if (!window->mInputCapture) return;

    if (state) {
        KeyPressedEvent evnt(key);
        Input::OnKey(key, true);
        EventManager::DispatchEvent(&evnt);
    } else {
        KeyReleasedEvent evnt(key);
        Input::OnKey(key, false);
        EventManager::DispatchEvent(&evnt);
    }
}

void WindowWayland::KeyboardModifiersEvent(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
    //GM_LOG_DEBUG("[Wayland] wl_keyboard.modifiers");
}

void WindowWayland::KeyboardRepeatInfoEvent(void *data, wl_keyboard *wl_keyboard, int32_t rate, int32_t delay) {
    //GM_LOG_DEBUG("[Wayland] wl_keyboard.repeat_info");
}

void WindowWayland::KeyboardKeymapEvent(void *data, wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size) {
    GM_LOG_DEBUG("[Wayland] wl_keyboard.keymap: Format={} Size={}", format, size);
    GM_VERIFY(format == XKB_KEYMAP_FORMAT_TEXT_V1);

    void* keymap = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    GM_VERIFY(keymap != MAP_FAILED);

    WindowWayland* window = (WindowWayland*)data;
    window->InitXKB((const char*)keymap);
    
    munmap(keymap, size);
    close(fd);
}

void WindowWayland::PointerEnterEvent(void *data, wl_pointer *pointer, uint32_t serial, wl_surface *surface, int32_t surface_x, int32_t surface_y) {
    GM_LOG_DEBUG("[Wayland] wl_pointer.enter: X={} Y={}", surface_x, surface_y);

    ((WindowWayland*)data)->mPointerLastSerial = serial;
}

void WindowWayland::PointerLeaveEvent(void *data, wl_pointer *pointer, uint32_t serial, wl_surface *surface) {
    GM_LOG_DEBUG("[Wayland] wl_pointer.leave");

    ((WindowWayland*)data)->mPointerLastSerial = 0;
}

void WindowWayland::PointerMotionEvent(void *data, wl_pointer *pointer, uint32_t time, int32_t surface_x, int32_t surface_y) {
    //GM_LOG_DEBUG("[Wayland] wl_pointer.motion: X={} Y={}", surface_x, surface_y);

    WindowWayland* window = (WindowWayland*)data;

    if (!window->mInputCapture) return;

    MouseMovedEvent evnt(surface_x, surface_y);
    EventManager::DispatchEvent(&evnt);
}

void WindowWayland::PointerButtonEvent(void *data, wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    //GM_LOG_DEBUG("[Wayland] wl_pointer.button: Button={:x} State={}", button, state);

     WindowWayland* window = (WindowWayland*)data;

    if (state) {
        uint32_t btn = (button-BTN_MOUSE+1) | GM_BUTTON_PREFIX;

        if (btn == GM_BUTTON_Left && !window->mInputCapture) {
            window->CaptureInput();
        }

        ButtonPressedEvent evnt(btn);
        EventManager::DispatchEvent(&evnt);
    } else {
        ButtonReleasedEvent evnt((button-BTN_MOUSE+1) | GM_BUTTON_PREFIX);
        EventManager::DispatchEvent(&evnt);
    }
}

void WindowWayland::PointerAxisEvent(void *data, wl_pointer *pointer, uint32_t time, uint32_t axis, int32_t value) {
    //GM_LOG_DEBUG("[Wayland] wl_pointer.axis: Axis={} Value={}", axis, value);
    
    WindowWayland* window = (WindowWayland*)data;

    if (!window->mInputCapture) return;

    // TODO: deal with later
}

void WindowWayland::PointerFrameEvent(void *data, wl_pointer *pointer) {
    //GM_LOG_DEBUG("[Wayland] wl_pointer.frame");
}

void WindowWayland::PointerAxisSourceEvent(void *data, wl_pointer *pointer, uint32_t axis_source) {
    //GM_LOG_DEBUG("[Wayland] wl_pointer.axis_source: Source={}", axis_source);
}

void WindowWayland::PointerAxisStopEvent(void *data, wl_pointer *pointer, uint32_t time, uint32_t axis) {
    //GM_LOG_DEBUG("[Wayland] wl_pointer.axis_stop: Axis={}", axis);
}

void WindowWayland::PointerAxisDiscreteEvent(void *data, wl_pointer *pointer, uint32_t axis, int32_t discrete) {
    //GM_LOG_DEBUG("[Wayland] wl_pointer.axis_discrete: Axis={} Discrete={}", axis, discrete);
}

void WindowWayland::PointerAxisValue120Event(void *data, wl_pointer *pointer, uint32_t axis, int32_t value120) {
    //GM_LOG_DEBUG("[Wayland] wl_pointer.axis_value120: Axis={} Value120={}", axis, value120);
}

void WindowWayland::PointerAxisRelativeDirectionEvent(void *data, wl_pointer *pointer, uint32_t axis, uint32_t direction) {
    //GM_LOG_DEBUG("[Wayland] wl_pointer.axis_relative_direction: Axis={} Direction={}", axis, direction);
}

void WindowWayland::PointerLockedEvent(void *data, zwp_locked_pointer_v1 *pointer) {
    GM_LOG_DEBUG("[Wayland] zwp_locked_pointer.locked");

    ((WindowWayland*)data)->mInputCapture = true;
}

void WindowWayland::PointerUnlockedEvent(void *data, zwp_locked_pointer_v1 *pointer) {
    GM_LOG_DEBUG("[Wayland] zwp_locked_pointer.unlocked");

    ((WindowWayland*)data)->mInputCapture = false;
}

void WindowWayland::PointerRelativeMotionEvent(void *data, zwp_relative_pointer_v1 *pointer, uint32_t utime_hi, uint32_t utime_lo, int32_t dx, int32_t dy, int32_t dx_unaccel, int32_t dy_unaccel) {
    //GM_LOG_DEBUG("[Wayland] zwp_relative_pointer.relative_motion: DX={} DY={} DXua={} DYua={}", dx, dy, dx_unaccel, dy_unaccel);

    WindowWayland* window = (WindowWayland*)data;

    if (!window->mInputCapture) return;

    // wl_fixed shift by 8 to ignore decimals
    MouseMovedEvent evnt(dx_unaccel >> 8, dy_unaccel >> 8);
    EventManager::DispatchEvent(&evnt);
}

void WindowWayland::DecorationTopLevelConfigure(void* data, zxdg_toplevel_decoration_v1* toplevel, uint32_t mode) {
    GM_LOG_DEBUG("[Wayland] zxdg_toplevel_decoration_v1.configure: Mode={}", mode);
}

}