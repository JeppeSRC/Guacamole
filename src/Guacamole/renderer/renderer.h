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

#include <Guacamole/vulkan/pipeline/pipeline.h>
#include <Guacamole/vulkan/renderpass.h>

namespace Guacamole { 

struct DescriptorUpdateBinding {
    uint32_t mBinding;
    uint32_t mArrayStart;
    uint32_t mCount; // has to be 1 atm

    VkDescriptorType mType;

    VkDescriptorBufferInfo mBufferInfo;
    VkDescriptorImageInfo mImageInfo;
};

class Renderer {
public:
    static void Init();
    static void Shutdown();

    static void BeginFrame();
    static void EndFrame();
    
    static void BindPipeline(const CommandBuffer* cmdBuffer, const Pipeline* pipeline);
    static void BeginRenderpass(const CommandBuffer* cmdBuffer, Renderpass* renderpass);
    static void EndRenderpass(const CommandBuffer* cmdBuffer, Renderpass* renderpass);

    static void UpdateDescriptorSet(const DescriptorSet& set, const DescriptorUpdateBinding* bindings, uint32_t count);
};

}