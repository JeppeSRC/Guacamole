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
    MousePressed, MouseReleased, MouseMoved, MouseScrolled,
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

class Window;
class EventManager {
public:
    static void Init(const Window* window);
    static void ProcessEvents(Window* window);
    static void Shutdown();

private:
#if defined(GM_LINUX)

    static xkb_context* mXkbContext;
    static int32_t mKeyboardID;
    static xkb_keymap* mKeymap;
    static xkb_state* mState;

#endif
};

class EventListener {
public:

};

}