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

#include "vec.h"

namespace Guacamole {

vec4::vec4(float val) : vec4(val, val, val, val) {}
vec4::vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
vec4::vec4(const vec2& o, float z, float w) : vec4(o.x, o.y, z, w) {}
vec4::vec4(const vec3& o, float w) : vec4(o.x, o.y, o.z, w) {}

vec4 vec4::operator=(const vec2& r) {
    __m128d tmp = _mm_load_sd((double*)&r.x);
    _mm_store_sd((double*)&x, *(__m128d*) & tmp);

    return *this;
}

vec4 vec4::operator=(const vec3& r) {
    __m128d tmp = _mm_load_sd((double*)&r.x);
    _mm_store_sd((double*)&x, *(__m128d*) & tmp);

    z = r.z;

    return *this;
}

vec4 vec4::operator+(const vec4& r) const {
    vec4 ret;
    __m128 left = _mm_loadu_ps(&x);
    __m128 right = _mm_loadu_ps(&r.x);

    __m128 result = _mm_add_ps(left, right);

    _mm_storeu_ps(&ret.x, result);

    return ret;
}

vec4 vec4::operator-(const vec4& r) const {
    vec4 ret;
    __m128 left = _mm_loadu_ps(&x);
    __m128 right = _mm_loadu_ps(&r.x);

    __m128 result = _mm_sub_ps(left, right);

    _mm_storeu_ps(&ret.x, result);

    return ret;
}

vec4 vec4::operator*(const vec4& r) const {
    vec4 ret;
    __m128 left = _mm_loadu_ps(&x);
    __m128 right = _mm_loadu_ps(&r.x);

    __m128 result = _mm_mul_ps(left, right);

    _mm_storeu_ps(&ret.x, result);

    return ret;
}

vec4 vec4::operator/(const vec4& r) const {
    vec4 ret;
    __m128 left = _mm_loadu_ps(&x);
    __m128 right = _mm_loadu_ps(&r.x);

    __m128 result = _mm_div_ps(left, right);

    _mm_storeu_ps(&ret.x, result);

    return ret;
}

vec4 vec4::operator-() const {
    vec4 ret;
    __m128 left = _mm_loadu_ps(&x);
    __m128 right = _mm_set_ps(-1.0f, -1.0f, -1.0f, -1.0f);

    __m128 result = _mm_add_ps(left, right);

    _mm_storeu_ps(&ret.x, result);

    return ret;
}


}