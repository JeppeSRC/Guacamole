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

struct xkb_context;
struct xkb_keymap;
struct xkb_state;

#endif

namespace Guacamole {

enum class EventType {
    None,
    KeyPressed, KeyReleased, 
    ButtonPressed, ButtonReleased, MouseMoved,
    WindowResize, WindowMove, WindowClose, WindowMinimize, WindowMaximize, WindowFocus, WindowFocusLost
};

class Event {
public:
    virtual EventType GetType() const = 0;
};

class KeyPressedEvent : public Event {
public:
    KeyPressedEvent(uint32_t key) : mKey(key) {}

    inline EventType GetType() const override { return EventType::KeyPressed; }
public:
    uint32_t mKey;
};

class KeyReleasedEvent : public Event {
public:
    KeyReleasedEvent(uint32_t key) : mKey(key) {}

    inline EventType GetType() const override { return EventType::KeyReleased; }

public:
    uint32_t mKey;
};

class ButtonPressedEvent : public Event {
public:
    ButtonPressedEvent(uint32_t button) : mButton(button) {}

    inline EventType GetType() const override { return EventType::ButtonPressed; }
public:
    uint32_t mButton;
};

class ButtonReleasedEvent : public Event {
public:
    ButtonReleasedEvent(uint32_t button) : mButton(button) {}

    inline EventType GetType() const override { return EventType::ButtonReleased; }

public:
    uint32_t mButton;
};

class MouseMovedEvent : public Event {
public:
    MouseMovedEvent(int32_t dx, int32_t dy) : mDeltaX(dx), mDeltaY(dy) {}

    inline EventType GetType() const override { return EventType::MouseMoved; }

public:
    int32_t mDeltaX;
    int32_t mDeltaY;
};

class WindowResizeEvent : public Event {
public:
    WindowResizeEvent(uint16_t width, uint16_t height) : mWidth(width), mHeight(height) {}

    inline EventType GetType() const override { return EventType::WindowResize; }
public:
    uint16_t mWidth;
    uint16_t mHeight;
};


class EventListener {
public:
    EventListener(EventType type) : mType(type) {}

    virtual bool OnEvent(Event* e) = 0;

public:
    EventType mType;
};

class Window;
class EventManager {
public:
    static void Init(Window* window);
    static void ProcessEvents(Window* window);
    static void Shutdown();


    template<typename T>
    static void AddListener(EventType type, T* object, bool(T::*callback)(Event*)) {
        mCallbacks.emplace_back(type, std::bind(callback, object, std::placeholders::_1));
    }

    static void AddListener(EventType type, bool(*callback)(Event*));

private:
    static void DispatchEvent(Event* e);
private:
    static std::vector<std::pair<EventType, std::function<bool(Event*)>>> mCallbacks;

    static int32_t mLastMouseX;
    static int32_t mLastMouseY;
    
    static Window* mWindow;
#if defined(GM_LINUX)
public:
    static xkb_state* GetState() { return mState; }

private:
    static xkb_context* mXkbContext;
    static int32_t mKeyboardID;
    static xkb_keymap* mKeymap;
    static xkb_state* mState;

#elif defined(GM_WINDOWS)
public:
    static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
    static void CheckButton(uint16_t buttonFlags, uint16_t down, uint16_t up, uint32_t keyCode);
#endif

    friend class Input;
};



}