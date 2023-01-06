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

#include <Guacamole/core/uuid.h>
#include <Guacamole/asset/asset.h>
#include <Guacamole/renderer/camera.h>
#include <Guacamole/core/math/vec.h>

namespace Guacamole {

struct IdComponent {
    UUID mUUID;
};

struct TagComponent {
    std::string mTag;
};

struct MeshComponent {
    AssetHandle mMesh;
};

struct SpriteComponent {
    AssetHandle mTexture;
};

struct TransformComponent {
    vec3 mTranslation;
    vec3 mRotation;
    vec3 mScale;

    mat4 GetTransform() const {
        return mat4::Translate(mTranslation) *
               mat4::RotateXY(mRotation) *
               mat4::Scale(mScale);
    }
};

struct CameraComponent {
    Camera mCamera;
    
    bool mPrimary;
  
};

struct MaterialComponent {
    AssetHandle mMaterial;
};

class NativeScript;
struct ScriptComponent {
    NativeScript* mScript = nullptr;

    NativeScript* (*CreateScript)();
    void (*DestroyScript)(ScriptComponent*);

    template<typename T>
    void Bind() {
        CreateScript = []() { return static_cast<NativeScript*>(new T()); };
        DestroyScript = [](ScriptComponent* comp) { delete comp->mScript; comp->mScript = nullptr; };
    }
};

}