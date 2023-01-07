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

#include "mat.h"
#include "math.h"

#if defined(GM_LINUX)
#define M128(reg, index) reg[index]
#elif defined(GM_WINDOWS)
#define M128(reg, index) reg.m128_f32[index]
#endif

namespace Guacamole {

mat4::mat4(float diag) {
    memset(m, 0, sizeof(m));

    m[0 + 0 * 4] = diag;
    m[1 + 1 * 4] = diag;
    m[2 + 2 * 4] = diag;
    m[3 + 3 * 4] = diag;
}

mat4::mat4(const mat4& o) {
    memcpy(m, o.m, sizeof(m));
}

mat4 mat4::Scale(const vec3& scale) {
    mat4 ret;

    MC(ret, 0, 0) = scale.x;
    MC(ret, 1, 1) = scale.y;
    MC(ret, 2, 2) = scale.z;

    return ret;
}

mat4 mat4::Translate(const vec3& translation) {
    mat4 ret(1.0f);

    MC(ret, 3, 0) = translation.x;
    MC(ret, 3, 1) = translation.y;
    MC(ret, 3, 2) = translation.z;

    return ret;
}

mat4 mat4::RotateXY(const vec3& rotation) {
    mat4 x, y;

    float xsin = sinf(rotation.x);
    float xcos = cosf(rotation.x);
    float ysin = sinf(rotation.y);
    float ycos = cosf(rotation.y);

    MC(x, 1, 1) = xcos;  MC(x, 2, 1) = -xsin;
    MC(x, 1, 2) = xsin;  MC(x, 2, 2) = xcos;

    MC(y, 0, 0) = ycos;  MC(y, 2, 0) = ysin;
    MC(y, 0, 2) = -ysin; MC(y, 2, 2) = ycos;

    return Mul_ColCol(x, y);
}

mat4 mat4::Perspective(float fov, float aspect, float zNear, float zFar) {
    mat4 ret(0.0f);

    float tanHalfFov = tan(GM_TO_RADIANS(fov) / 2.0f);

    MC(ret, 0, 0) = 1.0f / (aspect * tanHalfFov);
    MC(ret, 1, 1) = 1.0f / tanHalfFov;
    MC(ret, 2, 2) = zFar / (zFar - zNear);
    MC(ret, 3, 2) = -zNear * (zFar - zNear);
    MC(ret, 2, 3) = 1.0f;

    return ret;
}

mat4 mat4::Transpose(const mat4& m) {
    mat4 ret;

    for (uint32_t r = 0; r < 4; r++) {
        for (uint32_t c = 0; c < 4; c++) {
            MR(ret, c, r) = MC(m, c, r);
        }
    }

    return ret;
}

mat4 mat4::Inverse(const mat4& m) {
    mat4 ret;

   /*
    * m00 m10 m20 m30
    * m01 m11 m21 m31
    * m02 m12 m22 m32
    * m03 m13 m23 m33
    * */

#if defined(GM_MATH_PURE) || 1

#pragma region row0
    // MR is used to transpose it as it's written
    float m22m33 = MC(m, 2, 2) * MC(m, 3, 3);
    float m23m32 = MC(m, 2, 3) * MC(m, 3, 2);
    float m12m33 = MC(m, 1, 2) * MC(m, 3, 3);
    float m13m32 = MC(m, 1, 3) * MC(m, 3, 2);
    float m12m23 = MC(m, 1, 2) * MC(m, 2, 3);
    float m13m22 = MC(m, 1, 3) * MC(m, 2, 2);

    /*
    * m11 m21 m31
    * m12 m22 m32
    * m13 m23 m33
    * */

    MR(ret, 0, 0) = MC(m, 1, 1) * (m22m33 - m23m32) -
                    MC(m, 2, 1) * (m12m33 - m13m32) +
                    MC(m, 3, 1) * (m12m23 - m13m22);

    float m02m33 = MC(m, 0, 2) * MC(m, 3, 3);
    float m03m32 = MC(m, 0, 3) * MC(m, 3, 2);
    float m02m23 = MC(m, 0, 2) * MC(m, 2, 3);
    float m03m22 = MC(m, 0, 3) * MC(m, 2, 2);

   /*
    * m01 m21 m31
    * m02 m22 m32
    * m03 m23 m33
    * */

    MR(ret, 1, 0) = -(MC(m, 0, 1) * (m22m33 - m23m32) -
                      MC(m, 2, 1) * (m02m33 - m03m32) +
                      MC(m, 3, 1) * (m02m23 - m03m22));

    float m02m13 = MC(m, 0, 2) * MC(m, 1, 3);
    float m03m12 = MC(m, 0, 3) * MC(m, 1, 2);

   /*
    * m01 m11 m31
    * m02 m12 m32
    * m03 m13 m33
    * */

    MR(ret, 2, 0) = MC(m, 0, 1) * (m12m33 - m13m32) -
                    MC(m, 1, 1) * (m02m33 - m03m32) +
                    MC(m, 3, 1) * (m02m13 - m03m12);

   /*
    * m01 m11 m21
    * m02 m12 m22
    * m03 m13 m23
    * */

    MR(ret, 3, 0) = -(MC(m, 0, 1) * (m12m23 - m13m22) -
                      MC(m, 1, 1) * (m02m23 - m03m22) +
                      MC(m, 2, 1) * (m02m13 - m03m12));

#pragma endregion

#pragma region row1
    /*
    * m10 m20 m30
    * m12 m22 m32
    * m13 m23 m33
    * */

    MR(ret, 0, 1) = -(MC(m, 1, 0) * (m22m33 - m23m32) -
                      MC(m, 2, 0) * (m12m33 - m13m32) +
                      MC(m, 3, 0) * (m12m23 - m13m22));

   /*
    * m00 m20 m30
    * m02 m22 m32
    * m03 m23 m33
    * */


    MR(ret, 1, 1) = MC(m, 0, 0) * (m22m33 - m23m32) -
                    MC(m, 2, 0) * (m02m33 - m03m32) +
                    MC(m, 3, 0) * (m02m23 - m03m22);

   /*
    * m00 m10 m30
    * m02 m12 m32
    * m03 m13 m33
    * */

    MR(ret, 2, 1) = -(MC(m, 0, 0) * (m12m33 - m13m32) -
                      MC(m, 1, 0) * (m02m33 - m03m32) +
                      MC(m, 3, 0) * (m02m13 - m03m12));

   /*
    * m00 m10 m20 
    * m02 m12 m22 
    * m03 m13 m23 
    * */

    MR(ret, 3, 1) = MC(m, 0, 0) * (m12m23 - m13m22) -
                    MC(m, 1, 0) * (m02m23 - m03m22) +
                    MC(m, 2, 0) * (m02m13 - m03m12);

#pragma endregion

#pragma region row2

    float m21m33 = MC(m, 2, 1) * MC(m, 3, 3);
    float m23m31 = MC(m, 2, 3) * MC(m, 3, 1);
    float m11m33 = MC(m, 1, 1) * MC(m, 3, 3);
    float m13m31 = MC(m, 1, 3) * MC(m, 3, 1);
    float m11m23 = MC(m, 1, 1) * MC(m, 2, 3);
    float m13m21 = MC(m, 1, 3) * MC(m, 2, 1);

   /*
    * m10 m20 m30
    * m11 m21 m31
    * m13 m23 m33
    * */

    MR(ret, 0, 2) = MC(m, 1, 0) * (m21m33 - m23m31) -
                    MC(m, 2, 0) * (m11m33 - m13m31) +
                    MC(m, 3, 0) * (m11m23 - m13m21);


    float m01m33 = MC(m, 0, 1) * MC(m, 3, 3);
    float m03m31 = MC(m, 0, 3) * MC(m, 3, 1);
    float m01m23 = MC(m, 0, 1) * MC(m, 2, 3);
    float m03m21 = MC(m, 0, 3) * MC(m, 2, 1);

   /*
    * m00 m20 m30
    * m01 m21 m31
    * m03 m23 m33
    * */

    MR(ret, 1, 2) = -(MC(m, 0, 0) * (m21m33 - m23m31) -
                      MC(m, 2, 0) * (m01m33 - m03m31) +
                      MC(m, 3, 0) * (m01m23 - m03m21));

    float m01m13 = MC(m, 0, 1) * MC(m, 1, 3);
    float m03m11 = MC(m, 0, 3) * MC(m, 1, 1);

   /*
    * m00 m10 m30
    * m01 m11 m31
    * m03 m13 m33
    * */

    MR(ret, 2, 2) = MC(m, 0, 0) * (m11m33 - m13m31) -
                    MC(m, 1, 0) * (m01m33 - m03m31) +
                    MC(m, 3, 0) * (m01m13 - m03m11);

   /*
    * m00 m10 m20
    * m01 m11 m21
    * m03 m13 m23
    * */

    MR(ret, 3, 2) = -(MC(m, 0, 0) * (m11m23 - m13m21) -
                      MC(m, 1, 0) * (m01m23 - m03m21) +
                      MC(m, 2, 0) * (m01m13 - m03m11));


#pragma endregion

#pragma region row3

    float m21m32 = MC(m, 2, 1) * MC(m, 3, 2);
    float m22m31 = MC(m, 2, 2) * MC(m, 3, 1);
    float m11m32 = MC(m, 1, 1) * MC(m, 3, 2);
    float m12m31 = MC(m, 1, 2) * MC(m, 3, 1);
    float m11m22 = MC(m, 1, 1) * MC(m, 2, 2);
    float m12m21 = MC(m, 1, 2) * MC(m, 2, 1);

   /*
    * m10 m20 m30
    * m11 m21 m31
    * m12 m22 m32
    * */

    MR(ret, 0, 3) = -(MC(m, 1, 0) * (m21m32 - m22m31) -
                      MC(m, 2, 0) * (m11m32 - m12m31) +
                      MC(m, 3, 0) * (m11m22 - m12m21));

    float m01m32 = MC(m, 0, 1) * MC(m, 3, 2);
    float m02m31 = MC(m, 0, 2) * MC(m, 3, 1);
    float m01m22 = MC(m, 0, 1) * MC(m, 2, 2);
    float m02m21 = MC(m, 0, 2) * MC(m, 2, 1);
   /*
    * m00 m20 m30
    * m01 m21 m31
    * m02 m22 m32
    * */

    MR(ret, 1, 3) = MC(m, 0, 0) * (m21m32 - m22m31) -
                    MC(m, 2, 0) * (m01m32 - m02m31) +
                    MC(m, 3, 0) * (m01m22 - m02m21);

    float m01m12 = MC(m, 0, 1) * MC(m, 1, 2);
    float m02m11 = MC(m, 0, 2) * MC(m, 1, 1);
   /*
    * m00 m10 m30
    * m01 m11 m31
    * m02 m12 m32
    * */

    MR(ret, 2, 3) = -(MC(m, 0, 0) * (m11m32 - m12m31) -
                      MC(m, 1, 0) * (m01m32 - m02m31) +
                      MC(m, 3, 0) * (m01m12 - m02m11));

   /*
    * m00 m10 m20
    * m01 m11 m21
    * m02 m12 m22
    * */

    MR(ret, 3, 3) = MC(m, 0, 0) * (m11m22 - m12m21) -
                    MC(m, 1, 0) * (m01m22 - m02m21) +
                    MC(m, 2, 0) * (m01m12 - m02m11);

#pragma endregion


    float determinant = MC(m, 0, 0) * MR(ret, 0, 0) +
                        MC(m, 1, 0) * MR(ret, 1, 0) +
                        MC(m, 2, 0) * MR(ret, 2, 0) +
                        MC(m, 3, 0) * MR(ret, 3, 0);

    determinant = 1.0f / determinant;

    for (uint32_t i = 0; i < 16; i++) {
        ret[i] *= determinant;
    }

#else
/*
float m22m33 = MC(m, 2, 2) * MC(m, 3, 3);
float m23m32 = MC(m, 2, 3) * MC(m, 3, 2);
float m12m33 = MC(m, 1, 2) * MC(m, 3, 3);
float m13m32 = MC(m, 1, 3) * MC(m, 3, 2);
float m12m23 = MC(m, 1, 2) * MC(m, 2, 3);
float m13m22 = MC(m, 1, 3) * MC(m, 2, 2);
float m02m33 = MC(m, 0, 2) * MC(m, 3, 3);
float m03m32 = MC(m, 0, 3) * MC(m, 3, 2);
float m02m23 = MC(m, 0, 2) * MC(m, 2, 3);
float m03m22 = MC(m, 0, 3) * MC(m, 2, 2);
float m02m13 = MC(m, 0, 2) * MC(m, 1, 3);
float m03m12 = MC(m, 0, 3) * MC(m, 1, 2);
float m21m33 = MC(m, 2, 1) * MC(m, 3, 3);
float m23m31 = MC(m, 2, 3) * MC(m, 3, 1);
float m11m33 = MC(m, 1, 1) * MC(m, 3, 3);
float m13m31 = MC(m, 1, 3) * MC(m, 3, 1);
float m11m23 = MC(m, 1, 1) * MC(m, 2, 3);
float m13m21 = MC(m, 1, 3) * MC(m, 2, 1);
float m01m33 = MC(m, 0, 1) * MC(m, 3, 3);
float m03m31 = MC(m, 0, 3) * MC(m, 3, 1);
float m01m23 = MC(m, 0, 1) * MC(m, 2, 3);
float m03m21 = MC(m, 0, 3) * MC(m, 2, 1);
float m01m13 = MC(m, 0, 1) * MC(m, 1, 3);
float m03m11 = MC(m, 0, 3) * MC(m, 1, 1);
float m21m32 = MC(m, 2, 1) * MC(m, 3, 2);
float m22m31 = MC(m, 2, 2) * MC(m, 3, 1);
float m11m32 = MC(m, 1, 1) * MC(m, 3, 2);
float m12m31 = MC(m, 1, 2) * MC(m, 3, 1);
float m11m22 = MC(m, 1, 1) * MC(m, 2, 2);
float m12m21 = MC(m, 1, 2) * MC(m, 2, 1);
float m01m32 = MC(m, 0, 1) * MC(m, 3, 2);
float m02m31 = MC(m, 0, 2) * MC(m, 3, 1);
float m01m22 = MC(m, 0, 1) * MC(m, 2, 2);
float m02m21 = MC(m, 0, 2) * MC(m, 2, 1);
float m01m12 = MC(m, 0, 1) * MC(m, 1, 2);
float m02m11 = MC(m, 0, 2) * MC(m, 1, 1);
*/
#endif

    return ret;
}

void mat4::operator=(const mat4& r) {
    memcpy(m, r.m, sizeof(m));
}

mat4 mat4::operator*(const mat4& r) const {
    return Mul_ColCol(*this, r);
}

vec4 mat4::operator*(const vec4& r) const {
    vec4 ret;

#if defined(GM_MATH_PURE)
    ret.x = MC(m, 0, 0) * r.x + MC(m, 1, 0) * r.y + MC(m, 2, 0) * r.z + MC(m, 3, 0) * r.w;
    ret.y = MC(m, 0, 1) * r.x + MC(m, 1, 1) * r.y + MC(m, 2, 1) * r.z + MC(m, 3, 1) * r.w;
    ret.z = MC(m, 0, 2) * r.x + MC(m, 1, 2) * r.y + MC(m, 2, 2) * r.z + MC(m, 3, 2) * r.w;
    ret.w = MC(m, 0, 3) * r.x + MC(m, 1, 3) * r.y + MC(m, 2, 3) * r.z + MC(m, 3, 3) * r.w;
#else

    __m128 col0 = _mm_loadu_ps(m);
    __m128 col1 = _mm_loadu_ps(m+4);
    __m128 col2 = _mm_loadu_ps(m+8);
    __m128 col3 = _mm_loadu_ps(m+12);

    __m128 vecX = _mm_set_ps(r.x, r.x, r.x, r.x);
    __m128 vecY = _mm_set_ps(r.y, r.y, r.y, r.y);
    __m128 vecZ = _mm_set_ps(r.z, r.z, r.z, r.z);
    __m128 vecW = _mm_set_ps(r.w, r.w, r.w, r.w);

    __m128 res = _mm_mul_ps(col0, vecX);
    res = _mm_fmadd_ps(col1, vecY, res);
    res = _mm_fmadd_ps(col2, vecZ, res);
    res = _mm_fmadd_ps(col3, vecW, res);

    _mm_storeu_ps(&ret.x, res);

#endif

    return ret;
}

mat4 mat4::Mul_ColCol(const mat4&l, const mat4& r) {
    mat4 ret(0.0f);

#if defined(GM_MATH_PURE)

    for (uint32_t col = 0; col < 4; col++) {
        for (uint32_t row = 0; row < 4; row++) {
            for (uint32_t i = 0; i < 4; i++) {
                ret[row + col * 4] += l[row + i * 4] * r[i + col * 4];
            }
        }
    }

#else

    __m128 col0 = _mm_loadu_ps(r.m);
    __m128 col1 = _mm_loadu_ps(r.m + 4);
    __m128 col2 = _mm_loadu_ps(r.m + 8);
    __m128 col3 = _mm_loadu_ps(r.m + 12);

    {
    // First row
        __m128 row0 = _mm_set_ps(l.m[3 * 4], l.m[2 * 4], l.m[1 * 4], l.m[0]);

        __m128 m00 = _mm_mul_ps(row0, col0);
        __m128 m01 = _mm_mul_ps(row0, col1);
        __m128 m02 = _mm_mul_ps(row0, col2);
        __m128 m03 = _mm_mul_ps(row0, col3);

        __m128 m00_01 = _mm_hadd_ps(m00, m01);
        __m128 m02_03 = _mm_hadd_ps(m02, m03);
        __m128 res = _mm_hadd_ps(m00_01, m02_03);

        ret[0 + 0 * 4] = M128(res, 0);
        ret[0 + 1 * 4] = M128(res, 1);
        ret[0 + 2 * 4] = M128(res, 2);
        ret[0 + 3 * 4] = M128(res, 3);
    }
    {
    // Second row
        __m128 row1 = _mm_set_ps(l.m[1 + 3 * 4], l.m[1 + 2 * 4], l.m[1 + 1 * 4], l.m[1 + 0]);

        __m128 m10 = _mm_mul_ps(row1, col0);
        __m128 m11 = _mm_mul_ps(row1, col1);
        __m128 m12 = _mm_mul_ps(row1, col2);
        __m128 m13 = _mm_mul_ps(row1, col3);

        __m128 m10_11 = _mm_hadd_ps(m10, m11);
        __m128 m12_13 = _mm_hadd_ps(m12, m13);
        __m128 res = _mm_hadd_ps(m10_11, m12_13);

        ret[1 + 0 * 4] = M128(res, 0);
        ret[1 + 1 * 4] = M128(res, 1);
        ret[1 + 2 * 4] = M128(res, 2);
        ret[1 + 3 * 4] = M128(res, 3);
    }

    {
    // Third row
        __m128 row2 = _mm_set_ps(l.m[2 + 3 * 4], l.m[2 + 2 * 4], l.m[2 + 1 * 4], l.m[2 + 0]);

        __m128 m20 = _mm_mul_ps(row2, col0);
        __m128 m21 = _mm_mul_ps(row2, col1);
        __m128 m22 = _mm_mul_ps(row2, col2);
        __m128 m23 = _mm_mul_ps(row2, col3);

        __m128 m20_21 = _mm_hadd_ps(m20, m21);
        __m128 m22_23 = _mm_hadd_ps(m22, m23);
        __m128 res = _mm_hadd_ps(m20_21, m22_23);

        ret[2 + 0 * 4] = M128(res, 0);
        ret[2 + 1 * 4] = M128(res, 1);
        ret[2 + 2 * 4] = M128(res, 2);
        ret[2 + 3 * 4] = M128(res, 3);
    }

    {
// Fourth row
        __m128 row3 = _mm_set_ps(l.m[3 + 3 * 4], l.m[3 + 2 * 4], l.m[3 + 1 * 4], l.m[3 + 0]);

        __m128 m30 = _mm_mul_ps(row3, col0);
        __m128 m31 = _mm_mul_ps(row3, col1);
        __m128 m32 = _mm_mul_ps(row3, col2);
        __m128 m33 = _mm_mul_ps(row3, col3);

        __m128 m30_31 = _mm_hadd_ps(m30, m31);
        __m128 m32_33 = _mm_hadd_ps(m32, m33);
        __m128 res = _mm_hadd_ps(m30_31, m32_33);

        ret[3 + 0 * 4] = M128(res, 0);
        ret[3 + 1 * 4] = M128(res, 1);
        ret[3 + 2 * 4] = M128(res, 2);
        ret[3 + 3 * 4] = M128(res, 3);
    }

#endif

    return ret;
}

mat4 mat4::Mul_RowCol(const mat4& l, const mat4& r) {
    mat4 ret(0.0f);

#if defined(GM_MATH_PURE)

    for (uint32_t col = 0; col < 4; col++) {
        for (uint32_t row = 0; row < 4; row++) {
            for (uint32_t i = 0; i < 4; i++) {
                ret[row + col * 4] += l[i + row * 4] * r[i + col * 4];
            }
        }
    }

#else

    __m128 col0 = _mm_loadu_ps(r.m);
    __m128 col1 = _mm_loadu_ps(r.m + 4);
    __m128 col2 = _mm_loadu_ps(r.m + 8);
    __m128 col3 = _mm_loadu_ps(r.m + 12);

    // First row
    {
        __m128 row0 = _mm_loadu_ps(l.m);

        __m128 m00 = _mm_mul_ps(row0, col0);
        __m128 m01 = _mm_mul_ps(row0, col1);
        __m128 m02 = _mm_mul_ps(row0, col2);
        __m128 m03 = _mm_mul_ps(row0, col3);

        __m128 m00_01 = _mm_hadd_ps(m00, m01);
        __m128 m02_03 = _mm_hadd_ps(m02, m03);
        __m128 res = _mm_hadd_ps(m00_01, m02_03);

        ret[0 + 0 * 4] = M128(res, 0);
        ret[0 + 1 * 4] = M128(res, 1);
        ret[0 + 2 * 4] = M128(res, 2);
        ret[0 + 3 * 4] = M128(res, 3);
    }

    // Second row
    {
        __m128 row1 = _mm_loadu_ps(l.m+4);

        __m128 m10 = _mm_mul_ps(row1, col0);
        __m128 m11 = _mm_mul_ps(row1, col1);
        __m128 m12 = _mm_mul_ps(row1, col2);
        __m128 m13 = _mm_mul_ps(row1, col3);

        __m128 m10_11 = _mm_hadd_ps(m10, m11);
        __m128 m12_13 = _mm_hadd_ps(m12, m13);
        __m128 res = _mm_hadd_ps(m10_11, m12_13);

        ret[1 + 0 * 4] = M128(res, 0);
        ret[1 + 1 * 4] = M128(res, 1);
        ret[1 + 2 * 4] = M128(res, 2);
        ret[1 + 3 * 4] = M128(res, 3);
    }

    // Third row
    {
        __m128 row2 = _mm_loadu_ps(l.m+8);

        __m128 m20 = _mm_mul_ps(row2, col0);
        __m128 m21 = _mm_mul_ps(row2, col1);
        __m128 m22 = _mm_mul_ps(row2, col2);
        __m128 m23 = _mm_mul_ps(row2, col3);

        __m128 m20_21 = _mm_hadd_ps(m20, m21);
        __m128 m22_23 = _mm_hadd_ps(m22, m23);
        __m128 res = _mm_hadd_ps(m20_21, m22_23);

        ret[2 + 0 * 4] = M128(res, 0);
        ret[2 + 1 * 4] = M128(res, 1);
        ret[2 + 2 * 4] = M128(res, 2);
        ret[2 + 3 * 4] = M128(res, 3);
    }

    // Fourth row
    {
        __m128 row3 = _mm_loadu_ps(l.m+12);

        __m128 m30 = _mm_mul_ps(row3, col0);
        __m128 m31 = _mm_mul_ps(row3, col1);
        __m128 m32 = _mm_mul_ps(row3, col2);
        __m128 m33 = _mm_mul_ps(row3, col3);

        __m128 m30_31 = _mm_hadd_ps(m30, m31);
        __m128 m32_33 = _mm_hadd_ps(m32, m33);
        __m128 res = _mm_hadd_ps(m30_31, m32_33);

        ret[3 + 0 * 4] = M128(res, 0);
        ret[3 + 1 * 4] = M128(res, 1);
        ret[3 + 2 * 4] = M128(res, 2);
        ret[3 + 3 * 4] = M128(res, 3);
    }

#endif

    return ret;
}

}