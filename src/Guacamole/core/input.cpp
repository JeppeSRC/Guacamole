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

std::unordered_map<uint32_t, const char*> Input::mKeyStrings;
std::unordered_map<uint32_t, bool> Input::mKeys;

#define ADD(key, string) mKeyStrings[key] = string

void Input::Init() {
#pragma region
    ADD(GM_KEY_KP_Space, "Space");
    ADD(GM_KEY_KP_Tab, "Tab");
    ADD(GM_KEY_KP_Enter, "Enter");
    ADD(GM_KEY_KP_F1, "F1");
    ADD(GM_KEY_KP_F2, "F2");
    ADD(GM_KEY_KP_F3, "F3");
    ADD(GM_KEY_KP_F4, "F4");
    ADD(GM_KEY_KP_Home, "Home");
    ADD(GM_KEY_KP_Left, "Left");
    ADD(GM_KEY_KP_Up, "Up");
    ADD(GM_KEY_KP_Right, "Right");
    ADD(GM_KEY_KP_Down, "Down");
    ADD(GM_KEY_KP_Page_Up, "Page Up");
    ADD(GM_KEY_KP_Page_Down, "Page Down");
    ADD(GM_KEY_KP_End, "End");
    ADD(GM_KEY_KP_Insert, "Insert");
    ADD(GM_KEY_KP_Delete, "Del");
    ADD(GM_KEY_KP_Equal, "=");
    ADD(GM_KEY_KP_Multiply, "*");
    ADD(GM_KEY_KP_Add, "+");
    ADD(GM_KEY_KP_Separator, ",");
    ADD(GM_KEY_KP_Subtract, "-");
    ADD(GM_KEY_KP_Divide, "/");

    ADD(GM_KEY_KP_0, "0");
    ADD(GM_KEY_KP_1, "1");
    ADD(GM_KEY_KP_2, "2");
    ADD(GM_KEY_KP_3, "3");
    ADD(GM_KEY_KP_4, "4");
    ADD(GM_KEY_KP_5, "5");
    ADD(GM_KEY_KP_6, "6");
    ADD(GM_KEY_KP_7, "7");
    ADD(GM_KEY_KP_8, "8");
    ADD(GM_KEY_KP_9, "9");

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
    ADD(GM_KEY_F25, "F25");
    ADD(GM_KEY_F26, "F26");
    ADD(GM_KEY_F27, "F27");
    ADD(GM_KEY_F28, "F28");
    ADD(GM_KEY_F29, "F29");
    ADD(GM_KEY_F30, "F30");
    ADD(GM_KEY_F31, "F31");
    ADD(GM_KEY_F32, "F32");
    ADD(GM_KEY_F33, "F33");
    ADD(GM_KEY_F34, "F34");
    ADD(GM_KEY_F35, "F35");

    ADD(GM_KEY_Shift_L, "Left Shift");
    ADD(GM_KEY_Shift_R, " Right Shift");
    ADD(GM_KEY_Control_L, "Left Control");
    ADD(GM_KEY_Control_R, "Right Control");
    ADD(GM_KEY_Caps_Lock, "Caps Lock");
    ADD(GM_KEY_Shift_Lock, "Shift Lock");
    ADD(GM_KEY_Alt_L, "Left Alt");
    ADD(GM_KEY_Alt_R, "Right Alt");
    ADD(GM_KEY_Super_L, "Left Super");
    ADD(GM_KEY_Super_R, "Right Super");
    ADD(GM_KEY_Hyper_L, "Left Hyper");
    ADD(GM_KEY_Hyper_R, "Right Hyper");

    ADD(GM_KEY_Insert, "Insert");
    ADD(GM_KEY_Menu, "Menu");
    ADD(GM_KEY_Num_Lock, "Num Lock");

    ADD(GM_KEY_Home, "Home");
    ADD(GM_KEY_Left, "Left");
    ADD(GM_KEY_Up, "Up");
    ADD(GM_KEY_Right, "Right");
    ADD(GM_KEY_Down, "Down");
    ADD(GM_KEY_Page_Up, "Page Up");
    ADD(GM_KEY_Page_Down, "Page Down");
    ADD(GM_KEY_End, "End");

    ADD(GM_KEY_BackSpace, "Backspace");
    ADD(GM_KEY_Tab, "Tab");
    ADD(GM_KEY_Return, "Enter");
    ADD(GM_KEY_Pause, "Pause");
    ADD(GM_KEY_Scroll_Lock, "Scroll Lock");
    ADD(GM_KEY_Sys_Req, "SysReq");
    ADD(GM_KEY_Escape, "Escape");
    ADD(GM_KEY_Delete, "Del");


    ADD(GM_KEY_space, "Space");
    ADD(GM_KEY_exclam, "!");
    ADD(GM_KEY_quotedbl, "\"");
    ADD(GM_KEY_dollar, "$");
    ADD(GM_KEY_percent, "%");
    ADD(GM_KEY_ampersand, "&");
    ADD(GM_KEY_apostrophe, "'");
    ADD(GM_KEY_parenleft, "(");
    ADD(GM_KEY_parenright, ")");
    ADD(GM_KEY_asterisk, "*");
    ADD(GM_KEY_plus, "+");
    ADD(GM_KEY_comma, ",");
    ADD(GM_KEY_minus, "-");
    ADD(GM_KEY_period, ".");
    ADD(GM_KEY_slash, "/");
    ADD(GM_KEY_0, "0");
    ADD(GM_KEY_1, "1");
    ADD(GM_KEY_2, "2");
    ADD(GM_KEY_3, "3");
    ADD(GM_KEY_4, "4");
    ADD(GM_KEY_5, "5");
    ADD(GM_KEY_6, "6");
    ADD(GM_KEY_7, "7");
    ADD(GM_KEY_8, "8");
    ADD(GM_KEY_9, "9");
    ADD(GM_KEY_colon, ":");
    ADD(GM_KEY_semicolon, ";");
    ADD(GM_KEY_less, "<");
    ADD(GM_KEY_equal, "=");
    ADD(GM_KEY_greater, ">");
    ADD(GM_KEY_question, "?");
    ADD(GM_KEY_A, "A");
    ADD(GM_KEY_B, "B");
    ADD(GM_KEY_C, "C");
    ADD(GM_KEY_D, "D");
    ADD(GM_KEY_E, "E");
    ADD(GM_KEY_F, "F");
    ADD(GM_KEY_G, "G");
    ADD(GM_KEY_H, "H");
    ADD(GM_KEY_I, "I");
    ADD(GM_KEY_J, "J");
    ADD(GM_KEY_K, "K");
    ADD(GM_KEY_L, "L");
    ADD(GM_KEY_M, "M");
    ADD(GM_KEY_N, "N");
    ADD(GM_KEY_O, "O");
    ADD(GM_KEY_P, "P");
    ADD(GM_KEY_Q, "Q");
    ADD(GM_KEY_R, "R");
    ADD(GM_KEY_S, "S");
    ADD(GM_KEY_T, "T");
    ADD(GM_KEY_U, "U");
    ADD(GM_KEY_V, "V");
    ADD(GM_KEY_W, "W");
    ADD(GM_KEY_X, "X");
    ADD(GM_KEY_Y, "Y");
    ADD(GM_KEY_Z, "Z");
    ADD(GM_KEY_bracketleft, "[");
    ADD(GM_KEY_backslash, "\\");
    ADD(GM_KEY_bracketright, "]");
    ADD(GM_KEY_underscore, "_");
    ADD(GM_KEY_a, "a");
    ADD(GM_KEY_b, "b");
    ADD(GM_KEY_c, "c");
    ADD(GM_KEY_d, "d");
    ADD(GM_KEY_e, "e");
    ADD(GM_KEY_f, "f");
    ADD(GM_KEY_g, "g");
    ADD(GM_KEY_h, "h");
    ADD(GM_KEY_i, "i");
    ADD(GM_KEY_j, "j");
    ADD(GM_KEY_k, "k");
    ADD(GM_KEY_l, "l");
    ADD(GM_KEY_m, "m");
    ADD(GM_KEY_n, "n");
    ADD(GM_KEY_o, "o");
    ADD(GM_KEY_p, "p");
    ADD(GM_KEY_q, "q");
    ADD(GM_KEY_r, "r");
    ADD(GM_KEY_s, "s");
    ADD(GM_KEY_t, "t");
    ADD(GM_KEY_u, "u");
    ADD(GM_KEY_v, "v");
    ADD(GM_KEY_w, "w");
    ADD(GM_KEY_x, "x");
    ADD(GM_KEY_y, "y");
    ADD(GM_KEY_z, "z");
    ADD(GM_KEY_braceleft, "{");
    ADD(GM_KEY_bar, "|");
    ADD(GM_KEY_braceright, "}");
    ADD(GM_KEY_asciitilde, "~");
    ADD(GM_KEY_exclamdown, "¡");
    ADD(GM_KEY_sterling, "£");
    ADD(GM_KEY_diaeresis, "¨");
    ADD(GM_KEY_Adiaeresis, "Ä");
    ADD(GM_KEY_Aring, "Å");
    ADD(GM_KEY_Odiaeresis, "Ö");
    ADD(GM_KEY_multiply, "*");
    ADD(GM_KEY_adiaeresis, "ä");
    ADD(GM_KEY_aring, "å");
    ADD(GM_KEY_odiaeresis, "ö");
    ADD(GM_KEY_division, "\\");
#pragma endregion

    EventManager::AddListener(EventType::KeyPressed, OnKey);
    EventManager::AddListener(EventType::KeyReleased, OnKey);
}

void Input::Shutdown() {

}

bool Input::IsKeyPressed(uint32_t key) {
    auto it = mKeys.find(key);

    if (it == mKeys.end()) return false;

    return it->second;
}

const char* Input::GetKeyString(uint32_t key) {
    auto it = mKeyStrings.find(key);

    if (it == mKeyStrings.end()) {
        GM_LOG_WARNING("Key \"0x{:08x}\" doesn't have a string mapping", key);
        return "Unknown";
    }

    return it->second;
}

bool Input::OnKey(Event* event) {
    switch (event->GetType()) {
        case EventType::KeyPressed:
            mKeys[((KeyPressedEvent*)event)->mKey] = true;
            break;
        case EventType::KeyReleased:
            mKeys[((KeyReleasedEvent*)event)->mKey] = false;
            break;
    }

    return false;
}

}