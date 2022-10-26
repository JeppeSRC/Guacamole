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

namespace Guacamole {


enum class ShaderStage {
    Vertex,
    Fragment,
    Geometry,
    Compute,
};

enum class UniformType {
    Buffer,
    SampledImage
};

class UniformBaseType {
protected:
    UniformBaseType(UniformType type, const std::string& name, ShaderStage stage, uint32_t set, uint32_t binding)
        : mType(type), mName(name), mStage(stage), mSet(set), mBinding(binding) {}
public:
    UniformType mType;
    ShaderStage mStage;
    std::string mName;
    uint32_t mSet;
    uint32_t mBinding;
};

class UniformBufferType : public UniformBaseType {
public:
    // I will not bother with nested structs atm
    struct Member {
        std::string Name;
        uint32_t Size;
        uint32_t Offset;
    };

    UniformBufferType(const std::string& name, ShaderStage stage, uint32_t set, uint32_t binding, uint32_t size, std::vector<Member> members)
        : UniformBaseType(UniformType::Buffer, name, stage, set, binding), mSize(size), mMembers(members) {}

    uint32_t mSize;
    std::vector<Member> mMembers;
};

class SampledImageType : public UniformBaseType {
public:
    SampledImageType(const std::string& name, ShaderStage stage, uint32_t set, uint32_t binding, uint32_t arrayCount, spirv_cross::SPIRType::ImageType image)
        : UniformBaseType(UniformType::SampledImage, name, stage, set, binding), mArrayCount(arrayCount), mImage(image) {}

    uint32_t mArrayCount;
    spirv_cross::SPIRType::ImageType mImage;
};

}