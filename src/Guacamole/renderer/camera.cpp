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

#include "camera.h"

namespace Guacamole {

Camera::Camera() : mProjectionMatrix(1.0f), mViewMatrix(1.0f) {
    SetViewport(0, 0, 0, 0, 0.001f, 1.0f);
}

Camera::~Camera() {

}

void Camera::SetPerspective(float fov, float aspect, float zNear, float zFar) {
    mFov = fov;
    mAspect = aspect;
    mNear = zNear;
    mFar = zFar;

    mProjectionMatrix = mat4::Perspective(fov, aspect, zNear, zFar);
}

void Camera::SetAspect(float aspect) {
    SetPerspective(mFov, aspect, mNear, mFar);
}

void Camera::SetFov(float fov) {
    SetPerspective(fov, mAspect, mNear, mFar);
}

}