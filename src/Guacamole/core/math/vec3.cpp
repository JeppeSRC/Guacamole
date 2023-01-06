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

vec3::vec3(float val) : vec3(val, val, val) {}
vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}
vec3::vec3(const vec2& o, float z) : vec3(o.x, o.y, z) {}
vec3::vec3(const vec4& o) : vec3(o.x, o.y, o.z) {}

vec3 vec3::operator=(const vec2& r) {
    __m128d tmp = _mm_load_sd((double*)&r.x);
    _mm_store_sd((double*)&x, *(__m128d*)&tmp);

    return *this;
}

vec3 vec3::operator=(const vec4& r) {
    __m128d tmp = _mm_load_sd((double*)&r.x);
    _mm_store_sd((double*)&x, *(__m128d*)&tmp);
    z = r.z;

    return *this;
}

vec3 vec3::operator+(const vec3& r) const {
    vec3 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128d right = _mm_load_sd((double*)&r.x);

    __m128 result = _mm_add_ps(*(__m128*)&left, *(__m128*)&right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);
    ret.z = z + r.z;

    return ret;
}

vec3 vec3::operator-(const vec3& r) const {
    vec3 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128d right = _mm_load_sd((double*)&r.x);

    __m128 result = _mm_sub_ps(*(__m128*)&left, *(__m128*)&right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);
    ret.z = z - r.z;

    return ret;
}

vec3 vec3::operator*(const vec3& r) const {
    vec3 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128d right = _mm_load_sd((double*)&r.x);

    __m128 result = _mm_mul_ps(*(__m128*)&left, *(__m128*)&right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);
    ret.z = z * r.z;

    return ret;
}

vec3 vec3::operator/(const vec3& r) const {
    vec3 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128d right = _mm_load_sd((double*)&r.x);

    __m128 result = _mm_div_ps(*(__m128*)&left, *(__m128*)&right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);
    ret.z = z / r.z;

    return ret;
}

vec3 vec3::operator-() const {
    vec3 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128 right = _mm_set_ps(0, 0, -1.0f, -1.0f);

    __m128 result = _mm_mul_ps(*(__m128*)&left, right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);
    ret.z = z * -1.0f;

    return ret;
}


}