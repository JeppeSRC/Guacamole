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

#include "scene.h"
#include "components.h"

namespace Guacamole {
    
class Entity {
public:
    Entity(Scene* scene, entt::entity handle) : mScene(scene), mHandle(handle) {}

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args) {
        GM_ASSERT_MSG(!HasComponent<T>(), "Component already exist!");
        return mScene->mRegistry.emplace<T>(mHandle, args...);
    }

    template<typename T>
    bool HasComponent() {
        return mScene->mRegistry.all_of<T>(mHandle);
    }

    template<typename T>
    T& GetComponent() {
        GM_ASSERT_MSG(HasComponent<T>(), "Component doesn't exist!");
        return mScene->mRegistry.get<T>(mHandle);
    }

    template<typename T>
    void RemoveCompoent() {
        GM_ASSERT_MSG(HasComponent<T>(), "Component doesn't exist!");
        mScene->mRegistry.remove<T>(mHandle);
    }

private:
    entt::entity mHandle;
    Scene* mScene;
};

}