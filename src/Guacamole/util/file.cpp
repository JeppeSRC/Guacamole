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

#include "file.h"

namespace Guacamole {



File::File(const std::filesystem::path& path, bool open) : mPath(path), mSize(0), mHandle(nullptr){
    if (open) Open();
}

File::~File() {
    if (mHandle == nullptr) return;

    fclose(mHandle);
}

bool Guacamole::File::Open() {
    GM_ASSERT(mHandle == nullptr);
    mHandle = fopen(mPath.string().c_str(), "rb");

    if (mHandle == nullptr) {
        if (std::filesystem::exists(mPath)) {
            GM_LOG_CRITICAL("[File] Failed to open file \"{}\: file does not exist!", mPath.string().c_str());
        } else {
            GM_LOG_CRITICAL("[File] Failed to open file \"{}\!", mPath.string().c_str());
        }

        return false;
    }

    mSize = std::filesystem::file_size(mPath);

    return true;
}

void Guacamole::File::Close() {
    GM_ASSERT(mHandle != nullptr);

    fclose(mHandle);
    mHandle = nullptr;
}

uint64_t Guacamole::File::Write(void* src, uint64_t bytesToWrite, uint64_t offset) {
    if (offset != ~0)
        fseek(mHandle, offset, SEEK_SET);

    return fwrite(src, bytesToWrite, 1, mHandle);
}

uint64_t Guacamole::File::Read(void* dst, uint64_t bytesToRead, uint64_t offset) {
    if (offset != ~0) 
        fseek(mHandle, offset, SEEK_SET);

    return fread(dst, bytesToRead, 1, mHandle);
}

}