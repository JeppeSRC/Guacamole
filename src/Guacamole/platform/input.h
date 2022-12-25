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

namespace Guacamole {

#if defined(GM_LINUX)

#include <xkbcommon/xkbcommon-keysyms.h>

enum KeyCode {
    GM_KEY_KP_Enter = XKB_KEY_KP_Enter,

    GM_KEY_KP_Delete = XKB_KEY_KP_Separator,

    GM_KEY_KP_Multiply = XKB_KEY_KP_Multiply,
    GM_KEY_KP_Add = XKB_KEY_KP_Add,
    GM_KEY_KP_Subtract = XKB_KEY_KP_Subtract,
    GM_KEY_KP_Divide = XKB_KEY_KP_Divide,

    GM_KEY_KP_0 = XKB_KEY_KP_0,
    GM_KEY_KP_1 = XKB_KEY_KP_1,
    GM_KEY_KP_2 = XKB_KEY_KP_2,
    GM_KEY_KP_3 = XKB_KEY_KP_3,
    GM_KEY_KP_4 = XKB_KEY_KP_4,
    GM_KEY_KP_5 = XKB_KEY_KP_5,
    GM_KEY_KP_6 = XKB_KEY_KP_6,
    GM_KEY_KP_7 = XKB_KEY_KP_7,
    GM_KEY_KP_8 = XKB_KEY_KP_8,
    GM_KEY_KP_9 = XKB_KEY_KP_9,

    GM_KEY_F1 = XKB_KEY_F1,
    GM_KEY_F2 = XKB_KEY_F2,
    GM_KEY_F3 = XKB_KEY_F3,
    GM_KEY_F4 = XKB_KEY_F4,
    GM_KEY_F5 = XKB_KEY_F5,
    GM_KEY_F6 = XKB_KEY_F6,
    GM_KEY_F7 = XKB_KEY_F7,
    GM_KEY_F8 = XKB_KEY_F8,
    GM_KEY_F9 = XKB_KEY_F9,
    GM_KEY_F10 = XKB_KEY_F10,
    GM_KEY_F11 = XKB_KEY_F11,
    GM_KEY_F12 = XKB_KEY_F12,
    GM_KEY_F13 = XKB_KEY_F13,
    GM_KEY_F14 = XKB_KEY_F14,
    GM_KEY_F15 = XKB_KEY_F15,
    GM_KEY_F16 = XKB_KEY_F16,
    GM_KEY_F17 = XKB_KEY_F17,
    GM_KEY_F18 = XKB_KEY_F18,
    GM_KEY_F19 = XKB_KEY_F19,
    GM_KEY_F20 = XKB_KEY_F20,
    GM_KEY_F21 = XKB_KEY_F21,
    GM_KEY_F22 = XKB_KEY_F22,
    GM_KEY_F23 = XKB_KEY_F23,
    GM_KEY_F24 = XKB_KEY_F24,

    GM_KEY_Shift_L = XKB_KEY_Shift_L,
    GM_KEY_Shift_R = XKB_KEY_Shift_R,
    GM_KEY_Control_L = XKB_KEY_Control_L,
    GM_KEY_Control_R = XKB_KEY_Control_R,
    GM_KEY_Caps_Lock = XKB_KEY_Caps_Lock,
    GM_KEY_Shift_Lock = XKB_KEY_Shift_Lock,
    GM_KEY_Alt_L = XKB_KEY_Alt_L,
    GM_KEY_Alt_R = XKB_KEY_Alt_R,
    GM_KEY_Super_L = XKB_KEY_Super_L,
    GM_KEY_Super_R = XKB_KEY_Super_R,

    GM_KEY_Insert = XKB_KEY_Insert,
    GM_KEY_Menu = XKB_KEY_Menu,
    GM_KEY_Num_Lock = XKB_KEY_Num_Lock,

    GM_KEY_Home = XKB_KEY_Home,
    GM_KEY_Left = XKB_KEY_Left,
    GM_KEY_Up = XKB_KEY_Up,
    GM_KEY_Right = XKB_KEY_Right,
    GM_KEY_Down = XKB_KEY_Down,
    GM_KEY_Page_Up = XKB_KEY_Page_Up,
    GM_KEY_Page_Down = XKB_KEY_Page_Down,
    GM_KEY_End = XKB_KEY_End,
    GM_KEY_BackSpace = XKB_KEY_BackSpace,

    GM_KEY_Tab = XKB_KEY_Tab,
    GM_KEY_Return = XKB_KEY_Return,
    GM_KEY_Pause = XKB_KEY_Pause,
    GM_KEY_Scroll_Lock = XKB_KEY_Scroll_Lock,
    GM_KEY_Print = XKB_KEY_Sys_Req,
    GM_KEY_Escape = XKB_KEY_Escape,
    GM_KEY_Delete = XKB_KEY_Delete,

    GM_KEY_Space = XKB_KEY_space,

    GM_KEY_Plus = XKB_KEY_plus,
    GM_KEY_Comma = XKB_KEY_comma,
    GM_KEY_Minus = XKB_KEY_minus,
    GM_KEY_Period = XKB_KEY_period,

    GM_KEY_0 = XKB_KEY_0,
    GM_KEY_1 = XKB_KEY_1,
    GM_KEY_2 = XKB_KEY_2,
    GM_KEY_3 = XKB_KEY_3,
    GM_KEY_4 = XKB_KEY_4,
    GM_KEY_5 = XKB_KEY_5,
    GM_KEY_6 = XKB_KEY_6,
    GM_KEY_7 = XKB_KEY_7,
    GM_KEY_8 = XKB_KEY_8,
    GM_KEY_9 = XKB_KEY_9,

    GM_KEY_A = XKB_KEY_a,
    GM_KEY_B = XKB_KEY_b,
    GM_KEY_C = XKB_KEY_c,
    GM_KEY_D = XKB_KEY_d,
    GM_KEY_E = XKB_KEY_e,
    GM_KEY_F = XKB_KEY_f,
    GM_KEY_G = XKB_KEY_g,
    GM_KEY_H = XKB_KEY_h,
    GM_KEY_I = XKB_KEY_i,
    GM_KEY_J = XKB_KEY_j,
    GM_KEY_K = XKB_KEY_k,
    GM_KEY_L = XKB_KEY_l,
    GM_KEY_M = XKB_KEY_m,
    GM_KEY_N = XKB_KEY_n,
    GM_KEY_O = XKB_KEY_o,
    GM_KEY_P = XKB_KEY_p,
    GM_KEY_Q = XKB_KEY_q,
    GM_KEY_R = XKB_KEY_r,
    GM_KEY_S = XKB_KEY_s,
    GM_KEY_T = XKB_KEY_t,
    GM_KEY_U = XKB_KEY_u,
    GM_KEY_V = XKB_KEY_v,
    GM_KEY_W = XKB_KEY_w,
    GM_KEY_X = XKB_KEY_x,
    GM_KEY_Y = XKB_KEY_y,
    GM_KEY_Z = XKB_KEY_z,
    
};

#elif defined(GM_WINDOWS)


#include <WinUser.h>

#define GM_VK_EXTENDED 0xE000

enum KeyCode {
    GM_KEY_KP_Enter = VK_RETURN | GM_VK_EXTENDED,

    GM_KEY_KP_Delete = VK_DELETE,

    GM_KEY_KP_Multiply = VK_MULTIPLY,
    GM_KEY_KP_Add = VK_ADD,
    GM_KEY_KP_Subtract = VK_SUBTRACT,
    GM_KEY_KP_Divide = VK_DIVIDE | GM_VK_EXTENDED,

    GM_KEY_KP_0 = VK_INSERT,
    GM_KEY_KP_1 = VK_END,
    GM_KEY_KP_2 = VK_DOWN,
    GM_KEY_KP_3 = VK_NEXT,
    GM_KEY_KP_4 = VK_LEFT,
    GM_KEY_KP_5 = VK_CLEAR,
    GM_KEY_KP_6 = VK_RIGHT,
    GM_KEY_KP_7 = VK_HOME,
    GM_KEY_KP_8 = VK_UP,
    GM_KEY_KP_9 = VK_PRIOR,

    GM_KEY_F1 = VK_F1,
    GM_KEY_F2 = VK_F2,
    GM_KEY_F3 = VK_F3,
    GM_KEY_F4 = VK_F4,
    GM_KEY_F5 = VK_F5,
    GM_KEY_F6 = VK_F6,
    GM_KEY_F7 = VK_F7,
    GM_KEY_F8 = VK_F8,
    GM_KEY_F9 = VK_F9,
    GM_KEY_F10 = VK_F10,
    GM_KEY_F11 = VK_F11,
    GM_KEY_F12 = VK_F12,
    GM_KEY_F13 = VK_F13,
    GM_KEY_F14 = VK_F14,
    GM_KEY_F15 = VK_F15,
    GM_KEY_F16 = VK_F16,
    GM_KEY_F17 = VK_F17,
    GM_KEY_F18 = VK_F18,
    GM_KEY_F19 = VK_F19,
    GM_KEY_F20 = VK_F20,
    GM_KEY_F21 = VK_F21,
    GM_KEY_F22 = VK_F22,
    GM_KEY_F23 = VK_F23,
    GM_KEY_F24 = VK_F24,


    GM_KEY_Shift_L = VK_LSHIFT,
    GM_KEY_Shift_R = VK_RSHIFT,
    GM_KEY_Control_L = VK_LCONTROL,
    GM_KEY_Control_R = VK_RCONTROL | GM_VK_EXTENDED,
    GM_KEY_Caps_Lock = VK_CAPITAL,

    GM_KEY_Alt_L = VK_LMENU,
    GM_KEY_Alt_R = VK_RMENU | GM_VK_EXTENDED,
    GM_KEY_Super_L = VK_LWIN | GM_VK_EXTENDED,
    GM_KEY_Super_R = VK_RWIN | GM_VK_EXTENDED,


    GM_KEY_Insert = VK_INSERT | GM_VK_EXTENDED,
    GM_KEY_Menu = VK_APPS,
    GM_KEY_Num_Lock = VK_NUMLOCK,

    GM_KEY_Home = VK_HOME | GM_VK_EXTENDED,
    GM_KEY_Left = VK_LEFT | GM_VK_EXTENDED,
    GM_KEY_Up = VK_UP | GM_VK_EXTENDED,
    GM_KEY_Right = VK_RIGHT | GM_VK_EXTENDED,
    GM_KEY_Down = VK_DOWN | GM_VK_EXTENDED,
    GM_KEY_Page_Up = VK_PRIOR | GM_VK_EXTENDED,
    GM_KEY_Page_Down = VK_NEXT | GM_VK_EXTENDED,
    GM_KEY_End = VK_END | GM_VK_EXTENDED,
    GM_KEY_BackSpace = VK_BACK,

    GM_KEY_Tab = VK_TAB,
    GM_KEY_Return = VK_RETURN,
    GM_KEY_Pause = VK_PAUSE,
    GM_KEY_Scroll_Lock = VK_SCROLL,
    GM_KEY_Print = VK_PRINT,
    GM_KEY_Escape = VK_ESCAPE,
    GM_KEY_Delete = VK_DELETE | GM_VK_EXTENDED,

    GM_KEY_Space = VK_SPACE,
k,
    GM_KEY_Plus = VK_OEM_PLUS,
    GM_KEY_Comma = VK_OEM_COMMA,
    GM_KEY_Minus = VK_OEM_MINUS,
    GM_KEY_Period = VK_OEM_PERIOD,
h,
    GM_KEY_0 = 0x30,
    GM_KEY_1 = 0x31,
    GM_KEY_2 = 0x32,
    GM_KEY_3 = 0x33,
    GM_KEY_4 = 0x34,
    GM_KEY_5 = 0x35,
    GM_KEY_6 = 0x36,
    GM_KEY_7 = 0x37,
    GM_KEY_8 = 0x38,
    GM_KEY_9 = 0x39,

    GM_KEY_A = 0x41,
    GM_KEY_B = 0x42,
    GM_KEY_C = 0x43,
    GM_KEY_D = 0x44,
    GM_KEY_E = 0x45,
    GM_KEY_F = 0x46,
    GM_KEY_G = 0x47,
    GM_KEY_H = 0x48,
    GM_KEY_I = 0x49,
    GM_KEY_J = 0x4A,
    GM_KEY_K = 0x4B,
    GM_KEY_L = 0x4C,
    GM_KEY_M = 0x4D,
    GM_KEY_N = 0x4E,
    GM_KEY_O = 0x4F,
    GM_KEY_P = 0x50,
    GM_KEY_Q = 0x51,
    GM_KEY_R = 0x52,
    GM_KEY_S = 0x53,
    GM_KEY_T = 0x54,
    GM_KEY_U = 0x55,
    GM_KEY_V = 0x56,
    GM_KEY_W = 0x57,
    GM_KEY_X = 0x58,
    GM_KEY_Y = 0x59,
    GM_KEY_Z = 0x5A,

};

#endif

}