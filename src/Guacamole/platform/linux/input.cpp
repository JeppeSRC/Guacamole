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

#include <Guacamole/core/input.h>
#include <Guacamole/core/video/event.h>
#include <xkbcommon/xkbcommon-x11.h>

namespace Guacamole {

void Input::AddKey(uint32_t scanCode) {
    Key key;

    key.mKeyCode = (scanCode & 0x6900) ? scanCode & 0x0f : xkb_state_key_get_one_sym(EventManager::GetState(), scanCode);
    key.mScanCode = scanCode;
    key.mPressed = false;
    GetKeyString(key);

    mKeys[scanCode] = key;
    mScanCodes[key.mKeyCode] = scanCode;

    GM_LOG_DEBUG("Added Key: SC=0x{0:04x} KC=0x{1:04x} Desc='{2}'", scanCode, key.mKeyCode, key.mString);
}

void Input::GetKeyString(Key& key) {
    memset(key.mString, 0, MAX_KEY_STRING_LENGTH);

    #define STRING(str) memcpy(key.mString, str, strlen(str))
    auto it = mKeyCodeStrings.find(key.mKeyCode);

    if (it == mKeyCodeStrings.end()) {
        if (xkb_keysym_to_utf8(xkb_keysym_to_upper(key.mKeyCode), key.mString, MAX_KEY_STRING_LENGTH) <= 0) {
            GM_LOG_WARNING("Key: SC=0x{:04x} KC=0x{:04x} has no string mapping!", key.mScanCode, key.mKeyCode);
            STRING("Unknown");
            return;
        }

        return;
    }

    STRING(it->second);
}

}