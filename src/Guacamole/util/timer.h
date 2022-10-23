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

#include <chrono>

namespace Guacamole {

template<typename T = std::chrono::milliseconds>
class ScopedTimer {
public:
    ScopedTimer(const char* name) : mName(name), mStart(std::chrono::high_resolution_clock::now()) { }
    ~ScopedTimer() {
        auto end  = std::chrono::high_resolution_clock::now();

        uint64_t elapsed = std::chrono::duration_cast<T>(end - mStart).count();

        const char* suffix = "ms";

        if constexpr (std::is_same<T, std::chrono::microseconds>::value) {
            suffix = "us";
        } else if constexpr (std::is_same<T, std::chrono::nanoseconds>::value) {
            suffix = "ns";
        }

        GM_LOG_DEBUG("[ScopedTimer] {}: {}{}", mName, elapsed, suffix);
    }

public:
    const char* mName;
    std::chrono::high_resolution_clock::time_point mStart;

};


}