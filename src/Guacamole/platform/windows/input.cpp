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

namespace Guacamole {

void Input::AddKey(uint32_t scanCode) {
    Key key;

    key.mKeyCode = (scanCode & GM_BUTTON_PREFIX) == GM_BUTTON_PREFIX ? scanCode : MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
    key.mKeyCode |= (scanCode & 0xE100);
    key.mScanCode = scanCode;
    key.mPressed = false;
    GetKeyString(key);

    mKeys[scanCode] = key;
    mScanCodes[key.mKeyCode] = scanCode;

    GM_LOG_DEBUG("Added Key: SC=0x{0:04x} VK=0x{1:04x} Char='{2}'", scanCode, key.mKeyCode, key.mString);
}

void Input::GetKeyString(Key& key) {
    memset(key.mString, 0, MAX_KEY_STRING_LENGTH);

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
}

}