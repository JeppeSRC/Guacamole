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

#define GM_MATH_PUREs

#if !defined(GM_MATH_PURE)
#include <immintrin.h>
#endif

namespace Guacamole {

template<typename T>
class vec2_t {
public:
    T x, y;

    vec2_t(T value = (T)0) : vec2_t(value, value) {}
    vec2_t(T x, T y) : x(x), y(y) {}

#if defined(GM_MATH_PURE)
    inline vec2_t<T> operator+(const vec2_t<T> o) {
        return vec2_t<T>(x + o.x, y + o.y);
    }

    inline vec2_t<T> operator-(const vec2_t<T> o) {
        return vec2_t<T>(x - o.x, y - o.y);
    }

    inline vec2_t<T> operator*(const vec2_t<T> o) {
        return vec2_t<T>(x * o.x, y * o.y);
    }

    inline vec2_t<T> operator/(const vec2_t<T> o) {
        return vec2_t<T>(x / o.x, y / o.y);
    }

#else

    inline vec2_t<float> operator+(const vec2_t<float> o) {
        __m128 l = _mm_set_ps(0.0f, 0.0f, y, x);
        __m128 r = _mm_set_ps(0.0f, 0.0f, o.y, o.x);
        __m128 res = _mm_add_ps(l, r);

        alignas(16) float tmp[4];

        _mm_store_ps(tmp, res);

        vec2_t<float> ret;

        memcpy(&ret, &tmp, sizeof(ret));

        return ret;
    }

    inline vec2_t<float> operator-(const vec2_t<float> o) {
        return vec2_t<float>(x - o.x, y - o.y);
    }

    inline vec2_t<float> operator*(const vec2_t<float> o) {
        return vec2_t<float>(x * o.x, y * o.y);
    }

    inline vec2_t<float> operator/(const vec2_t<float> o) {
        return vec2_t<float>(x / o.x, y / o.y);
    }

#endif

    inline vec2_t<T>& operator+=(const vec2_t<T>& other) {
        return *this = *this + other;
    }

    inline vec2_t<T>& operator-=(const vec2_t<T>& other) {
        return *this = *this - other;
    }

    inline vec2_t<T>& operator*=(const vec2_t<T>& other) {
        return *this = *this * other;
    }

    inline vec2_t<T>& operator/=(const vec2_t<T>& other) {
        return *this = *this / other;
    }

};

typedef vec2_t<float> vec2;



}