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

#include "shader.h"
#include "renderpass.h"
#include "pipelinelayout.h"

namespace Guacamole {

class Pipeline {
public:
    virtual ~Pipeline();

    inline VkPipeline GetHandle() const { return mPipelineHandle; }
protected:
    VkPipeline mPipelineHandle;

};

struct GraphicsPipelineInfo {
    uint32_t mWidth;
    uint32_t mHeight;

    std::vector<VkVertexInputBindingDescription> mVertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> mVertexInputAttributes;
    PipelineLayout* mPipelineLayout;
    Renderpass* mRenderpass;
    
    Shader* mShader;
};

class GraphicsPipeline : public Pipeline {
public:
    GraphicsPipeline(const GraphicsPipelineInfo& info);

private:
    GraphicsPipelineInfo mInfo;
};

}