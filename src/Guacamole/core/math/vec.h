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

namespace Guacamole {

class vec2 {
public:
    float x;
    float y;

    vec2(float val = 0.0f);
    vec2(float x, float y);

#if defined(GM_MATH_PURE)

    vec2 operator+(const vec2& r) const {
        return vec2(x + r.x, y + r.y);
    }

    vec2 operator-(const vec2& r) const {
        return vec2(x - r.x, y - r.y);
    }

    vec2 operator*(const vec2& r) const {
        return vec2(x * r.x, y * r.y);
    }

    vec2 operator/(const vec2& r) const {
        return vec2(x / r.x, y / r.y);
    }

    vec2 operator-() const {
        return vec2(-x, -y);
    }

#else
    vec2 operator+(const vec2& r) const;
    vec2 operator-(const vec2& r) const;
    vec2 operator*(const vec2& r) const;
    vec2 operator/(const vec2& r) const;
    vec2 operator-() const;

#endif

    vec2& operator+=(const vec2& r) {
        return *this = operator+(r);
    }

    vec2& operator-=(const vec2& r) {
        return *this = operator-(r);
    }

    vec2& operator*=(const vec2& r) {
        return *this = operator*(r);
    }

    vec2& operator/=(const vec2& r) {
        return *this = operator/(r);
    }
};

class vec4;
class vec3 {
public:
    float x;
    float y;
    float z;

    vec3(float val = 0.0f);
    vec3(float x, float y, float z);
    vec3(const vec2& o, float z = 0.0f);
    vec3(const vec4& o);

#if defined(GM_MATH_PURE)

    vec3 operator=(const vec2& r) {
        x = r.x;
        y = r.y;

        return *this;
    }

    vec3 operator=(const vec4& r) {
        x = r.x;
        y = r.y;
        z = r.z;
        return *this;
    }

    vec3 operator+(const vec3& r) const {
        return vec3(x + r.x, y + r.y, z + r.z);
    }

    vec3 operator-(const vec3& r) const {
        return vec3(x - r.x, y - r.y, z - r.z);
    }

    vec3 operator*(const vec3& r) const {
        return vec3(x * r.x, y * r.y, z * r.z);
    }

    vec3 operator/(const vec3& r) const {
        return vec3(x / r.x, y / r.y, z / r.z);
    }

    vec3 operator-() const {
        return vec3(-x, -y, -z);
    }

#else
    vec3 operator=(const vec2& r);
    vec3 operator=(const vec4& r);
    vec3 operator+(const vec3& r) const;
    vec3 operator-(const vec3& r) const;
    vec3 operator*(const vec3& r) const;
    vec3 operator/(const vec3& r) const;
    vec3 operator-() const;
#endif

    vec3& operator+=(const vec3& r) {
        return *this = operator+(r);
    }

    vec3& operator-=(const vec3& r) {
        return *this = operator-(r);
    }

    vec3& operator*=(const vec3& r) {
        return *this = operator*(r);
    }

    vec3& operator/=(const vec3& r) {
        return *this = operator/(r);
    }
};

class vec4 {
public:
    float x;
    float y;
    float z;
    float w;

    vec4(float val = 0.0f);
    vec4(float x, float y, float z, float w);
    vec4(const vec2& o, float z = 0.0f, float w = 0.0f);
    vec4(const vec3& o, float w = 0.0f);

#if defined(GM_MATH_PURE)

    vec4 operator=(const vec2& r) {
        x = r.x;
        y = r.y;
        return *this;
    }

    vec4 operator=(const vec3& r) {
        x = r.x;
        y = r.y;
        z = r.z;
        return *this;
    }

    vec4 operator+(const vec4& r) const {
        return vec4(x + r.x, y + r.y, z + r.z, w + r.w);
    }

    vec4 operator-(const vec4& r) const {
        return vec4(x - r.x, y - r.y, z - r.z, w - r.w);
    }

    vec4 operator*(const vec4& r) const {
        return vec4(x * r.x, y * r.y, z * r.z, w * r.w);
    }

    vec4 operator/(const vec4& r) const {
        return vec4(x / r.x, y / r.y, z / r.z, w / r.w);
    }

    vec4 operator-() const {
        return vec4(-x, -y, -z, -w);
    }

#else
    vec4 operator=(const vec2& r);
    vec4 operator=(const vec3& r);
    vec4 operator+(const vec4& r) const;
    vec4 operator-(const vec4& r) const;
    vec4 operator*(const vec4& r) const;
    vec4 operator/(const vec4& r) const;
    vec4 operator-() const;
#endif

    vec4& operator+=(const vec4& r) {
        return *this = operator+(r);
    }

    vec4& operator-=(const vec4& r) {
        return *this = operator-(r);
    }

    vec4& operator*=(const vec4& r) {
        return *this = operator*(r);
    }

    vec4& operator/=(const vec4& r) {
        return *this = operator/(r);
    }
};


}