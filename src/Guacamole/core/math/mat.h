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

#define MC(m, col, row) m[row + col * 4]
#define MR(m, col, row) m[col + row * 4]

namespace Guacamole {

class mat4 {
public:
    union {
        vec4 col[4];
        float m[16];
    };

    mat4(float diag = 1.0f);
    mat4(const mat4& o);

    inline float& operator[](uint64_t index) { return m[index]; }
    inline float operator[](uint64_t index) const { return m[index]; }
    inline vec4& Col(uint64_t index) { return col[index]; }
    inline vec4 Col(uint64_t index) const { return col[index]; }

    void operator=(const mat4& r);
    mat4 operator*(const mat4& r) const;
    vec4 operator*(const vec4& r) const;

    static mat4 Scale(const vec3& scale);
    static mat4 Translate(const vec3& translation);
    static mat4 RotateXY(const vec3& rotation);
    static mat4 Perspective(float fov, float aspect, float zNear, float zFar);
    static mat4 Transpose(const mat4& m);
    static mat4 Inverse(const mat4& m);
    static mat4 Mul_ColCol(const mat4& l, const mat4& r);
    static mat4 Mul_RowCol(const mat4& l, const mat4& r);
};

}