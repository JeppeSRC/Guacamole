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

#include "renderer.h"

#include <Guacamole/vulkan/renderpass.h>

namespace Guacamole {

void Renderer::Init() {

}

void Renderer::Shutdown() {

}

void Renderer::BeginFrame() {

}

void Renderer::EndFrame() {

}

void Renderer::BindPipeline(const CommandBuffer* cmdBuffer, const Pipeline* pipeline) {
    vkCmdBindPipeline(cmdBuffer->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetHandle());
}

void Renderer::BeginRenderpass(const CommandBuffer* cmdBuffer, Renderpass* renderpass) {
    renderpass->Begin(cmdBuffer);
}

void Renderer::EndRenderpass(const CommandBuffer* cmdBuffer, Renderpass* renderpass) {
    renderpass->End(cmdBuffer);
}

void Renderer::UpdateDescriptorSet(const DescriptorSet& set, const DescriptorUpdateBinding* bindings, uint32_t count) {
    GM_ASSERT(count < 16);
    GM_ASSERT(bindings != nullptr);
    VkWriteDescriptorSet write[16];

    for (uint32_t i = 0; i < count; i++) {
        const DescriptorUpdateBinding& binding = bindings[i];
        
        GM_VERIFY(write[i].descriptorCount == 1);

        write[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write[i].pNext = nullptr;
        write[i].dstSet = set.GetHandle();
        write[i].dstBinding = binding.mBinding;
        write[i].dstArrayElement = binding.mArrayStart;
        write[i].descriptorCount = binding.mCount;
        write[i].descriptorType = binding.mType;
        write[i].pBufferInfo = &binding.mBufferInfo;
        write[i].pImageInfo = &binding.mImageInfo;
    }

    //vkUpdateDescriptorSets(Context::GetDeviceHandle(), count, write, 0, nullptr);
}

}