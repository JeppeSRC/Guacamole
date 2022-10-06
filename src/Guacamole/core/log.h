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

#include <spdlog/spdlog.h>


#if defined(GM_WINDOWS)
#define FUNCSIG __FUNCSIG__
#else
#define FUNCSIG __PRETTY_FUNCTION__ //For now we just assume it's defined on non msvc compilers
#endif


#if defined(GM_LINUX)

#if defined(GM_DEBUG)

#define GM_ASSERT(cond) if (!(cond)) { spdlog::critical("Assertion Failed:\n" \
                                                        "\tCondition: {0}\n" \
                                                        "\tFile: {1}\n"\
                                                        "\tFunction: {2}\n"\
                                                        "\tLine: {3}", #cond, __FILE__, FUNCSIG, __LINE__);\
                                                        __builtin_trap(); }

#define GM_ASSERT_MSG(cond, msg) if (!(cond)) { spdlog::critical("Assertion Failed: {4}\n" \
                                                        "\tCondition: {0}\n" \
                                                        "\tFile: {1}\n"\
                                                        "\tFunction: {2}\n"\
                                                        "\tLine: {3}", #cond, __FILE__, FUNCSIG, __LINE__, msg);\
                                                        __builtin_trap(); }

#else

#define GM_ASSERT(cond)
#define GM_ASSERT_MSG(cond, msg)

#endif



#define GM_VERIFY(cond) if (!(cond)) { spdlog::critical("Verification Failed:\n" \
                                                        "\tCondition: {0}\n" \
                                                        "\tFile: {1}\n"\
                                                        "\tFunction: {2}\n"\
                                                        "\tLine: {3}", #cond, __FILE__, FUNCSIG, __LINE__);\
                                                        __builtin_trap(); }

#define GM_VERIFY_MSG(cond, msg) if (!(cond)) { spdlog::critical("Verification Failed: {4}\n" \
                                                        "\tCondition: {0}\n" \
                                                        "\tFile: {1}\n"\
                                                        "\tFunction: {2}\n"\
                                                        "\tLine: {3}", #cond, __FILE__, FUNCSIG, __LINE__, msg);\
                                                        __builtin_trap(); }


#define GM_LOG_INFO(message...) spdlog::info(message)
#define GM_LOG_WARNING(message...) spdlog::warn(message)
#define GM_LOG_DEBUG(message...) spdlog::debug(message)
#define GM_LOG_CRITICAL(message...) spdlog::critical(message)
#elif defined(GM_WINDOWS)

#if defined(GM_DEBUG)

#define GM_ASSERT(cond) if (!(cond)) { spdlog::critical("Assertion Failed:\n" \
                                                        "\tCondition: {0}\n" \
                                                        "\tFile: {1}\n"\
                                                        "\tFunction: {2}\n"\
                                                        "\tLine: {3}", #cond, __FILE__, FUNCSIG, __LINE__);\
                                                        __debugbreak(); }

#define GM_ASSERT_MSG(cond, msg) if (!(cond)) { spdlog::critical("Assertion Failed: {4}\n" \
                                                        "\tCondition: {0}\n" \
                                                        "\tFile: {1}\n"\
                                                        "\tFunction: {2}\n"\
                                                        "\tLine: {3}", #cond, __FILE__, FUNCSIG, __LINE__, msg);\
                                                        __debugbreak(); }

#else

#define GM_ASSERT(cond)
#define GM_ASSERT_MSG(cond, msg)

#endif

#define GM_VERIFY(cond) if (!(cond)) { spdlog::critical("Verification Failed:\n" \
                                                        "\tCondition: {0}\n" \
                                                        "\tFile: {1}\n"\
                                                        "\tFunction: {2}\n"\
                                                        "\tLine: {3}", #cond, __FILE__, FUNCSIG, __LINE__);\
                                                        __debugbreak(); }

#define GM_VERIFY_MSG(cond, msg) if (!(cond)) { spdlog::critical("Verification Failed: {4}\n" \
                                                        "\tCondition: {0}\n" \
                                                        "\tFile: {1}\n"\
                                                        "\tFunction: {2}\n"\
                                                        "\tLine: {3}", #cond, __FILE__, FUNCSIG, __LINE__, msg);\
                                                        __debugbreak(); }

#define GM_LOG_INFO(message,...) spdlog::info(message, __VA_ARGS__)
#define GM_LOG_WARNING(message,...) spdlog::warn(message, __VA_ARGS__)
#define GM_LOG_DEBUG(message,...) spdlog::debug(message, __VA_ARGS__)
#define GM_LOG_CRITICAL(message,...) spdlog::critical(message, __VA_ARGS__)
#endif

#ifdef GM_DEBUG
#define VK(call) GMCheckVkCall(call, __FILE__, #call, __FUNCTION__, __LINE__)
#else
#define VK(call) call
#endif

namespace Guacamole {


}