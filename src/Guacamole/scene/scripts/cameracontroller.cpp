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

#include "cameracontroller.h"

#include <Guacamole/core/input.h>

namespace Guacamole { namespace Script {

bool CameraController::OnWindowResize(WindowResizeEvent* e) {
    CameraComponent& camComp = GetComponent<CameraComponent>();

    float width = (float)e->mWidth;
    float height = (float)e->mHeight;

    camComp.mCamera.SetViewport(width, height);
    camComp.mCamera.SetAspect(width / height);

    return false;
}

bool CameraController::OnMouseMoved(MouseMovedEvent* e) {
    TransformComponent& trans = GetComponent<TransformComponent>();
    
    trans.mRotation.y += (float)e->mDeltaX * mSensitivity;
    trans.mRotation.x += (float)e->mDeltaY * -mSensitivity;

    return false;
}

void CameraController::OnCreate() {
    AddEvent(EventType::WindowResize);
    AddEvent(EventType::MouseMoved);

    mSensitivity = 0.001f;
    mSpeed = 1.5f;

    mForward = Input::GetScanCode(GM_KEY_W);
    mBack = Input::GetScanCode(GM_KEY_S);
    mRight = Input::GetScanCode(GM_KEY_D);
    mLeft = Input::GetScanCode(GM_KEY_A);
    mUp = Input::GetScanCode(GM_KEY_Space);
    mDown = Input::GetScanCode(GM_KEY_Shift_L);
}

void CameraController::OnUpdate(float ts) {
    TransformComponent& transform = GetComponent<TransformComponent>();
    Camera& camera = GetComponent<CameraComponent>().mCamera;

    UpdateCamera(ts, true, camera, transform);
}

void CameraController::UpdateCamera(float ts, bool lockY, Camera& camera, TransformComponent& transform) {
    glm::mat4 rot(1.0f);
    rot = glm::rotate(rot, transform.mRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    rot = glm::rotate(rot, transform.mRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));


    glm::vec4 adj = glm::vec4(mSpeed, mSpeed, mSpeed, 0.0f) * ts;
    glm::mat4 inverse = glm::inverse(rot);
    glm::vec3 forward = inverse[2] * adj;
    glm::vec3 right = inverse[0] * adj;
    glm::vec3 up = inverse[1] * adj;

    if (lockY) {
        up = glm::vec3(0.0f, mSpeed * ts, 0.0f);
        forward.y = 0.0f;
    }

    if (Input::IsKeyPressed(mForward))
        transform.mTranslation -= forward;
    else if (Input::IsKeyPressed(mBack))
        transform.mTranslation += forward;

    if (Input::IsKeyPressed(mRight))
        transform.mTranslation += right;
    else if (Input::IsKeyPressed(mLeft))
        transform.mTranslation -= right;

    if (Input::IsKeyPressed(mUp)) {
        transform.mTranslation -= up;
    } else if (Input::IsKeyPressed(mDown)) {
        transform.mTranslation += up;
    }

    camera.SetView(glm::translate(rot, -transform.mTranslation));
}

} }