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
#include <Guacamole/core/math/math.h>
#include <Guacamole/util/timer.h>

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
    
    trans.mRotation.y -= (float)e->mDeltaX * mSensitivity;
    trans.mRotation.x += (float)e->mDeltaY * mSensitivity;

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
    mat4 rot(1.0f);

    rot = mat4::RotateXY(transform.mRotation);

    vec4 adj = vec4(mSpeed, mSpeed, mSpeed, 0.0f) * ts;
    mat4 inverse = mat4::Inverse(rot);
    vec3 forward = inverse.Col(2) * adj;
    vec3 right = inverse.Col(0) * adj;
    vec3 up = inverse.Col(1) * adj;

    if (lockY) {
        up = vec3(0.0f, mSpeed * ts, 0.0f);
        forward.y = 0.0f;
    }

    if (Input::IsKeyPressed(mForward))
        transform.mTranslation += forward;
    else if (Input::IsKeyPressed(mBack))
        transform.mTranslation -= forward;

    if (Input::IsKeyPressed(mRight))
        transform.mTranslation += right;
    else if (Input::IsKeyPressed(mLeft))
        transform.mTranslation -= right;

    if (Input::IsKeyPressed(mUp)) {
        transform.mTranslation -= up;
    } else if (Input::IsKeyPressed(mDown)) {
        transform.mTranslation += up;
    }

    camera.SetView(rot * mat4::Translate(-transform.mTranslation));
}

} }