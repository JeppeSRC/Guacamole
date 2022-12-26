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
std::unordered_map<uint32_t, uint32_t> Input::mScanCodes;
std::unordered_map<uint32_t, const char*> Input::mKeyCodeStrings;

#define ADD(key, string) mKeyCodeStrings[key] = string
#define ADD_RANGE(start, end) for (uint32_t i = (start); i <= (end); i++) { AddKey(i); }

void Input::Init() {

    ADD(GM_KEY_BackSpace, "BACKSPACE");
    ADD(GM_KEY_Tab, "TAB");
    ADD(GM_KEY_End, "ENTER");
    ADD(GM_KEY_KP_Enter, "KP ENTER");
    ADD(GM_KEY_Caps_Lock, "CAPS");
    ADD(GM_KEY_Escape, "ESCAPE");
    ADD(GM_KEY_Space, "SPACE");
    ADD(GM_KEY_KP_0, "KP 0");
    ADD(GM_KEY_KP_1, "KP 1");
    ADD(GM_KEY_KP_2, "KP 2");
    ADD(GM_KEY_KP_3, "KP 3");
    ADD(GM_KEY_KP_4, "KP 4");
    ADD(GM_KEY_KP_5, "KP 5");
    ADD(GM_KEY_KP_6, "KP 6");
    ADD(GM_KEY_KP_7, "KP 7");
    ADD(GM_KEY_KP_8, "KP 8");
    ADD(GM_KEY_KP_9, "KP 9");
    ADD(GM_KEY_KP_Delete, "KP DEL");
    ADD(GM_KEY_Page_Up, "PAGE UP");
    ADD(GM_KEY_Page_Down, "PAGE DOWN");
    ADD(GM_KEY_End, "END");
    ADD(GM_KEY_Home, "HOME");
    ADD(GM_KEY_Left, "LEFT");
    ADD(GM_KEY_Up, "UP");
    ADD(GM_KEY_Right, "RIGHT");
    ADD(GM_KEY_Down, "DOWN");
    ADD(GM_KEY_Insert, "INSERT");
    ADD(GM_KEY_Delete, "DEL");
    ADD(GM_KEY_Print, "PRINT SCREEN");
    ADD(GM_KEY_Super_L, "L SUPER");
    ADD(GM_KEY_Super_R, "R SUPER");
    ADD(GM_KEY_Menu, "APPS");
    ADD(GM_KEY_KP_Multiply, "KP *");
    ADD(GM_KEY_KP_Add, "KP +");
    ADD(GM_KEY_KP_Subtract, "KP -");
    ADD(GM_KEY_KP_Divide, "KP /");
    ADD(GM_KEY_F1, "F1");
    ADD(GM_KEY_F2, "F2");
    ADD(GM_KEY_F3, "F3");
    ADD(GM_KEY_F4, "F4");
    ADD(GM_KEY_F5, "F5");
    ADD(GM_KEY_F6, "F6");
    ADD(GM_KEY_F7, "F7");
    ADD(GM_KEY_F8, "F8");
    ADD(GM_KEY_F9, "F9");
    ADD(GM_KEY_F10, "F10");
    ADD(GM_KEY_F11, "F11");
    ADD(GM_KEY_F12, "F12");
    ADD(GM_KEY_F13, "F13");
    ADD(GM_KEY_F14, "F14");
    ADD(GM_KEY_F15, "F15");
    ADD(GM_KEY_F16, "F16");
    ADD(GM_KEY_F17, "F17");
    ADD(GM_KEY_F18, "F18");
    ADD(GM_KEY_F19, "F19");
    ADD(GM_KEY_F20, "F20");
    ADD(GM_KEY_F21, "F21");
    ADD(GM_KEY_F22, "F22");
    ADD(GM_KEY_F23, "F23");
    ADD(GM_KEY_F24, "F24");
    ADD(GM_KEY_Num_Lock, "NUM LOCK");
    ADD(GM_KEY_Scroll_Lock, "SCROLL LOCK");
    ADD(GM_KEY_Shift_L, "L SHIFT");
    ADD(GM_KEY_Shift_R, "R SHIFT");
    ADD(GM_KEY_Control_L, "L CTRL");
    ADD(GM_KEY_Control_R, "R CTRL");
    ADD(GM_KEY_Alt_L, "L ALT");
    ADD(GM_KEY_Alt_R, "R ALT");
    ADD(GM_KEY_Plus, "+");
    ADD(GM_KEY_Comma, ",");
    ADD(GM_KEY_Minus, "-");
    ADD(GM_KEY_Period, ".");

    ADD(GM_BUTTON_Left, "MB LEFT");
    ADD(GM_BUTTON_Middle, "MB MIDDLE");
    ADD(GM_BUTTON_Right, "MB RIGHT");
    ADD(GM_BUTTON_Scroll_Up, "MB SCROLL UP");
    ADD(GM_BUTTON_Scroll_Down, "MB SCROLL DOWN");
    ADD(GM_BUTTON_Back, "MB BACK");
    ADD(GM_BUTTON_Forward, "MB FORWARD");

#if defined(GM_WINDOWS)
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

#elif defined(GM_LINUX)

     // Scancodes 0x01 to 0x58 are offset +8 for some reason
    ADD_RANGE(0x09, 0x5c);
    ADD_RANGE(0x5e, 0x60);

    // Escaped scancodes starting with 0xE000 are mapped from 0x68
    ADD_RANGE(0x68, 0x6A);
    ADD_RANGE(0x6E, 0x77);
    ADD_RANGE(0x85, 0x87);

    ADD_RANGE(GM_BUTTON_OFFSET | 0x01, GM_BUTTON_OFFSET | 0x05);
    ADD_RANGE(GM_BUTTON_OFFSET | 0x08, GM_BUTTON_OFFSET | 0x09);

#endif
}

void Input::Shutdown() {

}

bool Input::IsVKeyPressed(uint32_t keyCode) {
    uint32_t scanCode = mScanCodes[keyCode];
    return IsKeyPressed(scanCode);
}

bool Input::IsKeyPressed(uint32_t scanCode) {
    auto it = mKeys.find(scanCode);

    if (it == mKeys.end()) return false;

    return it->second.mPressed;
}

const Input::Key* Input::GetKeyInfo(uint32_t scanCode) {
    auto it = mKeys.find(scanCode);
    
    GM_VERIFY_MSG(it != mKeys.end(), "ScanCode doesn't exist!");

    return &it->second;
}

void Input::OnKey(uint32_t scanCode, bool pressed) {
    mKeys[scanCode].mPressed = pressed;
}

}