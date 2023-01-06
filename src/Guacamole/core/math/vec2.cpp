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

vec2::vec2(float val) : vec2(val, val) {}
vec2::vec2(float x, float y) : x(x), y(y) {}

vec2 vec2::operator+(const vec2& r) const {
    vec2 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128d right = _mm_load_sd((double*)&r.x);

    __m128 result = _mm_add_ps(*(__m128*)&left, *(__m128*)&right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);

    return ret;
}

vec2 vec2::operator-(const vec2& r) const {
    vec2 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128d right = _mm_load_sd((double*)&r.x);

    __m128 result = _mm_sub_ps(*(__m128*)&left, *(__m128*)&right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);

    return ret;
}

vec2 vec2::operator*(const vec2& r) const {
    vec2 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128d right = _mm_load_sd((double*)&r.x);

    __m128 result = _mm_mul_ps(*(__m128*)&left, *(__m128*)&right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);

    return ret;
}

vec2 vec2::operator/(const vec2& r) const {
    vec2 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128d right = _mm_load_sd((double*)&r.x);

    __m128 result = _mm_div_ps(*(__m128*)&left, *(__m128*)&right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);

    return ret;
}

vec2 vec2::operator-() const {
    vec2 ret;
    __m128d left = _mm_load_sd((double*)&x);
    __m128 right = _mm_set_ps(0, 0, -1.0f, -1.0f);

    __m128 result = _mm_mul_ps(*(__m128*)&left, right);

    _mm_store_sd((double*)&ret.x, *(__m128d*)&result);

    return ret;
}


}