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
#include <glm/ext.hpp>

namespace Guacamole {

class Camera {
public:
    Camera();
    ~Camera();

    inline const glm::mat4& GetProjection() const { return mProjectionMatrix; }
    inline const glm::mat4& GetView() const { return mViewMatrix; }
    inline void SetView(const glm::mat4& view) { mViewMatrix = view;  }

    inline void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) { 
        mViewport = { x, y, width, height, minDepth, maxDepth }; 
    }

    inline void SetViewport(float width, float height) { 
        mViewport.width = width; mViewport.height = height; 
    }

    void SetPerspective(float fov, float aspect, float zNear, float zFar);
    void SetAspect(float aspect);
    void SetFov(float fov);

    inline const VkViewport& GetViewport() const { return mViewport; }

protected:
    VkViewport mViewport;

    float mFov;
    float mAspect;
    float mNear;
    float mFar;

    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;

};

}