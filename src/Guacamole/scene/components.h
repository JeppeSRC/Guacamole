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

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Guacamole/core/uuid.h>
#include <Guacamole/asset/asset.h>
#include <Guacamole/renderer/camera.h>

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
    glm::vec3 mTranslation;
    glm::vec3 mRotation;
    glm::vec3 mScale;

    glm::mat4 GetTransform() const {
        return glm::translate(glm::mat4(1.0f), mTranslation) *
               glm::toMat4(glm::quat(mRotation)) *
               glm::scale(glm::mat4(1.0f), mScale);
    }

    glm::mat4 GetView() const {
        return glm::translate(glm::mat4(1.0f), -mTranslation) *
               glm::toMat4(glm::quat(mRotation)) *
               glm::scale(glm::mat4(1.0f), mScale);
    }
};

struct CameraComponent {
    Camera mCamera;
    
    bool mPrimary;
  
};

struct MaterialComponent {
    AssetHandle mMaterial;
};

class Entity;
struct ScriptComponent {
    typedef void(*FPN_ScriptUpdate)(Entity* entity, float ts);

    ScriptComponent(FPN_ScriptUpdate fn) : mScript(fn) {}


    FPN_ScriptUpdate mScript;
};

}