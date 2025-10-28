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

#include <unordered_map>

#include <Guacamole/core/video/event.h>

namespace Guacamole {

class Input {
public:
    static constexpr uint32_t MAX_KEY_STRING_LENGTH = 32;

    struct Key {
        uint32_t mKeyCode;
        uint32_t mScanCode;
        bool mPressed;
        char mString[MAX_KEY_STRING_LENGTH];
    };

public:
    static void Init();
    static void Shutdown();

    static bool IsKeyPressed(uint32_t scanCode);
    static bool IsVKeyPressed(uint32_t keyCode);
    static uint32_t GetScanCode(uint32_t keyCode);
    static const Key* GetKeyInfo(uint32_t scanCode);

private:
    static void OnKey(uint32_t scanCode, bool pressed);
    static void AddKey(uint32_t scanCode);
    static void GetKeyString(Key& key);

    static std::unordered_map<uint32_t, Key> mKeys;
    static std::unordered_map<uint32_t, uint32_t> mScanCodes;
    static std::unordered_map<uint32_t, const char*> mKeyCodeStrings;

    friend class WindowXCB;
    friend class WindowWindows;
};

}