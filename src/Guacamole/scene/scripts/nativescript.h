/*
MIT License
a
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

#include <Guacamole/scene/entity.h>

namespace Guacamole {

class NativeScript {
public:
    virtual ~NativeScript() {}

    bool OnEvent(Event*);

protected:
    virtual bool OnKeyPressed(KeyPressedEvent*) { return false; }
    virtual bool OnKeyReleased(KeyReleasedEvent*) { return false; }
    virtual bool OnButtonPressed(ButtonPressedEvent*) { return false; }
    virtual bool OnButtonReleased(ButtonReleasedEvent*) { return false; }
    virtual bool OnMouseMoved(MouseMovedEvent*) { return false; }
    virtual bool OnWindowResize(WindowResizeEvent*) { return false; }

    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual void OnUpdate(float ts) {}

    template<typename T>
    T& GetComponent() {
        return mEntity.GetComponent<T>();
    }

    void AddEvent(EventType type);

private:
    Entity mEntity;
    friend class Scene;
};

}