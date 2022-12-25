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

#include "input.h"

namespace Guacamole {

std::unordered_map<uint32_t, Input::Key> Input::mKeys;
std::unordered_map<uint32_t, const char*> Input::mKeyCodeStrings;

#define ADD_RANGE(start, end) for (uint32_t i = start; i <= end; i++) { AddKey(i); }
#define ADD(key, string) mKeyCodeStrings[key] = string

void Input::Init() {


#if defined(GM_WINDOWS)
    ADD(VK_CANCEL | GM_VK_EXTENDED, "CTRL+BREAK");
    ADD(VK_BACK, "BACKSPACE");
    ADD(VK_TAB, "TAB");
    ADD(VK_RETURN, "ENTER");
    ADD(VK_RETURN | GM_VK_EXTENDED, "KP ENTER");
    ADD(VK_CAPITAL, "CAPS");
    ADD(VK_ESCAPE, "ESCAPE");
    ADD(VK_SPACE, "SPACE");
    ADD(VK_CLEAR, "KP 5");
    ADD(VK_PRIOR, "KP 9");
    ADD(VK_NEXT, "KP 3");
    ADD(VK_END, "KP 1");
    ADD(VK_HOME, "KP 7");
    ADD(VK_LEFT, "KP 4");
    ADD(VK_UP, "KP 8");
    ADD(VK_RIGHT, "KP 6");
    ADD(VK_DOWN, "KP 2");
    ADD(VK_INSERT, "KP 0");
    ADD(VK_DELETE, "KP ,");
    ADD(VK_PRIOR | GM_VK_EXTENDED, "PAGE UP");
    ADD(VK_NEXT | GM_VK_EXTENDED, "PAGE DOWN");
    ADD(VK_END | GM_VK_EXTENDED, "END");
    ADD(VK_HOME | GM_VK_EXTENDED, "HOME");
    ADD(VK_LEFT | GM_VK_EXTENDED, "LEFT");
    ADD(VK_UP | GM_VK_EXTENDED, "UP");
    ADD(VK_RIGHT | GM_VK_EXTENDED, "RIGHT");
    ADD(VK_DOWN | GM_VK_EXTENDED, "DOWN");
    ADD(VK_INSERT | GM_VK_EXTENDED, "INSERT");
    ADD(VK_DELETE | GM_VK_EXTENDED, "DEL");
    ADD(VK_SNAPSHOT | GM_VK_EXTENDED, "PRINT SCREEN");
    ADD(VK_SNAPSHOT, "ALT+SYSREQ");
    ADD(VK_LWIN | GM_VK_EXTENDED, "L SUPER");
    ADD(VK_RWIN | GM_VK_EXTENDED, "R SUPER");
    ADD(VK_APPS | GM_VK_EXTENDED, "APPS");
    ADD(VK_MULTIPLY, "KP *");
    ADD(VK_ADD, "KP +");
    ADD(VK_SUBTRACT, "KP -");
    ADD(VK_DIVIDE | GM_VK_EXTENDED, "KP /");
    ADD(VK_F1, "F1");
    ADD(VK_F2, "F2");
    ADD(VK_F3, "F3");
    ADD(VK_F4, "F4");
    ADD(VK_F5, "F5");
    ADD(VK_F6, "F6");
    ADD(VK_F7, "F7");
    ADD(VK_F8, "F8");
    ADD(VK_F9, "F9");
    ADD(VK_F10, "F10");
    ADD(VK_F11, "F11");
    ADD(VK_F12, "F12");
    ADD(VK_F13, "F13");
    ADD(VK_F14, "F14");
    ADD(VK_F15, "F15");
    ADD(VK_F16, "F16");
    ADD(VK_F17, "F17");
    ADD(VK_F18, "F18");
    ADD(VK_F19, "F19");
    ADD(VK_F20, "F20");
    ADD(VK_F21, "F21");
    ADD(VK_F22, "F22");
    ADD(VK_F23, "F23");
    ADD(VK_F24, "F24");
    ADD(VK_NUMLOCK, "NUM LOCK");
    ADD(VK_SCROLL, "SCROLL LOCK");
    ADD(VK_LSHIFT, "L SHIFT");
    ADD(VK_RSHIFT, "R SHIFT");
    ADD(VK_LCONTROL, "L CTRL");
    ADD(VK_RCONTROL | GM_VK_EXTENDED, "R CTRL");
    ADD(VK_LMENU, "L ALT");
    ADD(VK_RMENU | GM_VK_EXTENDED, "R ALT");
    ADD(VK_OEM_PLUS, "+");
    ADD(VK_OEM_COMMA, ",");
    ADD(VK_OEM_MINUS, "-");
    ADD(VK_OEM_PLUS, "?");
    ADD(VK_OEM_PERIOD, ".");

#endif

    ADD_RANGE(0x1, 0x58);
    ADD_RANGE(0xE01C, 0xE01D);
    AddKey(0xE035);
    ADD_RANGE(0xE037, 0xE038);
    ADD_RANGE(0xE046, 0xE049);
    AddKey(0xE04B);
    AddKey(0xE04D);
    AddKey(0xE04F);
    ADD_RANGE(0xE050, 0xE053);
    ADD_RANGE(0xE05B, 0xE05D);
}

void Input::Shutdown() {

}

bool Input::IsKeyPressed(uint32_t scanCode) {
    auto it = mKeys.find(scanCode);

    if (it == mKeys.end()) return false;

    return it->second.mPressed;
}

bool Input::IsVKeyPressed(uint32_t keyCode) {
    uint32_t scanCode;
#if defined(GM_WINDOWS)
    scanCode = MapVirtualKey(keyCode, MAPVK_VK_TO_VSC_EX);
#endif

    return IsKeyPressed(scanCode);
}

const Input::Key* Input::GetKeyInfo(uint32_t scanCode) {
    auto it = mKeys.find(scanCode);
    
    GM_VERIFY_MSG(it != mKeys.end(), "ScanCode doesn't exist!");

    return &it->second;
}

void Input::OnKey(uint32_t scanCode, bool pressed) {
    mKeys[scanCode].mPressed = pressed;
}


void Input::AddKey(uint32_t scanCode) {
    Key key;

#if defined(GM_WINDOWS)
    key.mKeyCode = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
    key.mKeyCode |= (scanCode & 0xE100);
#endif
    key.mScanCode = scanCode;
    key.mPressed = false;
    GetKeyString(key);

    mKeys[scanCode] = key;

    GM_LOG_DEBUG("Added Key: SC=0x{0:04x} VK=0x{1:04x} Char='{2}'", scanCode, key.mKeyCode, key.mString);
}


void Input::GetKeyString(Key& key) {
    memset(key.mString, 0, MAX_KEY_STRING_LENGTH);

#if defined(GM_WINDOWS)
#define STRING(str) memcpy(key.mString, str, strlen(str))
    auto it = mKeyCodeStrings.find(key.mKeyCode);

    if (it == mKeyCodeStrings.end()) {
        uint16_t val = MapVirtualKey(key.mKeyCode, MAPVK_VK_TO_CHAR) & 0xFFFF;

        if (val == 0) {
            GM_LOG_WARNING("Key: SC=0x{:04x} KC=0x{:04x} has no string mapping!", key.mScanCode, key.mKeyCode);
            STRING("Unknown");
            return;
        }

        key.mString[0] = (char)val;
        return;
    }

    STRING(it->second);
#endif

}

}