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
#include "pipeline.h"

#include <Guacamole/vulkan/device.h>

namespace Guacamole {

Pipeline::~Pipeline() {
    vkDestroyPipeline(mDevice->GetHandle(), mPipelineHandle, nullptr);
}

GraphicsPipeline::GraphicsPipeline(Device* device, const GraphicsPipelineInfo& info) : Pipeline(device), mInfo(info) {

    VkPipelineShaderStageCreateInfo ssInfo[2];

    ssInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ssInfo[0].pNext = nullptr;
    ssInfo[0].flags = 0;
    ssInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    ssInfo[0].module = info.mShader->GetHandle(ShaderStage::Vertex);
    ssInfo[0].pName = "main";
    ssInfo[0].pSpecializationInfo = nullptr;

    ssInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ssInfo[1].pNext = nullptr;
    ssInfo[1].flags = 0;
    ssInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ssInfo[1].module = info.mShader->GetHandle(ShaderStage::Fragment);
    ssInfo[1].pName = "main";
    ssInfo[1].pSpecializationInfo = nullptr;
  
    VkPipelineVertexInputStateCreateInfo viInfo;

    viInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    viInfo.pNext = nullptr;
    viInfo.flags = 0;
    viInfo.vertexBindingDescriptionCount = (uint32_t)info.mVertexInputBindings.size();
    viInfo.pVertexBindingDescriptions = info.mVertexInputBindings.data();
    viInfo.vertexAttributeDescriptionCount = (uint32_t)info.mVertexInputAttributes.size();
    viInfo.pVertexAttributeDescriptions = info.mVertexInputAttributes.data();

    VkPipelineInputAssemblyStateCreateInfo iasInfo;

    iasInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    iasInfo.pNext = nullptr;
    iasInfo.flags = 0;
    iasInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    iasInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo vsInfo;

    vsInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vsInfo.pNext = nullptr;
    vsInfo.flags = 0;
    vsInfo.viewportCount = 1;

    VkViewport viewPort;

    viewPort.width = (float)info.mWidth;
    viewPort.height = (float)info.mHeight;
    viewPort.x = 0;
    viewPort.y = 0;
    viewPort.minDepth = 0.001f;
    viewPort.maxDepth = 1.0f;

    vsInfo.pViewports = &viewPort;
    vsInfo.scissorCount = 1;

    VkRect2D scissor;

    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = info.mWidth;
    scissor.extent.height = info.mHeight;

    vsInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rsInfo;

    rsInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rsInfo.pNext = nullptr;
    rsInfo.flags = 0;
    rsInfo.depthClampEnable = VK_FALSE;
    rsInfo.rasterizerDiscardEnable = false;
    rsInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rsInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rsInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rsInfo.depthBiasEnable = false;
    rsInfo.depthBiasConstantFactor = 0.0f;
    rsInfo.depthBiasClamp = false;
    rsInfo.depthBiasSlopeFactor = 0.0f;
    rsInfo.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo msInfo;

    msInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msInfo.pNext = nullptr;
    msInfo.flags = 0;
    msInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    msInfo.sampleShadingEnable = false;
    msInfo.minSampleShading = 0.0f;
    msInfo.pSampleMask = nullptr;
    msInfo.alphaToCoverageEnable = false;
    msInfo.alphaToOneEnable = false;

    VkPipelineDepthStencilStateCreateInfo dssInfo;

    dssInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    dssInfo.pNext = nullptr;
    dssInfo.flags = 0;
    dssInfo.depthTestEnable = true;
    dssInfo.depthWriteEnable = true;
    dssInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    dssInfo.depthBoundsTestEnable = false;
    dssInfo.stencilTestEnable = false;
    dssInfo.front.failOp = VK_STENCIL_OP_ZERO;
    dssInfo.front.passOp = VK_STENCIL_OP_KEEP;
    dssInfo.front.depthFailOp = VK_STENCIL_OP_ZERO;
    dssInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;
    dssInfo.front.compareMask = 0;
    dssInfo.front.writeMask = 0;
    dssInfo.front.reference = 0;
    dssInfo.back.failOp = VK_STENCIL_OP_ZERO;
    dssInfo.back.passOp = VK_STENCIL_OP_KEEP;
    dssInfo.back.depthFailOp = VK_STENCIL_OP_ZERO;
    dssInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    dssInfo.back.compareMask = 0;
    dssInfo.back.writeMask = 0;
    dssInfo.back.reference = 0;
    dssInfo.minDepthBounds = 0.0f;
    dssInfo.maxDepthBounds = 0.0f;
    

    VkPipelineColorBlendAttachmentState basInfo;

    basInfo.blendEnable = false;
    basInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    basInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    basInfo.colorBlendOp = VK_BLEND_OP_ADD;
    basInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    basInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    basInfo.alphaBlendOp = VK_BLEND_OP_ADD;
    basInfo.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;

    VkPipelineColorBlendStateCreateInfo bsInfo;

    bsInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    bsInfo.pNext = nullptr;
    bsInfo.flags = 0;
    bsInfo.logicOpEnable = false;
    bsInfo.logicOp = VK_LOGIC_OP_COPY;
    bsInfo.attachmentCount = 1;
    bsInfo.pAttachments = &basInfo;

    VkPipelineDynamicStateCreateInfo dsInfo;

    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    dsInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dsInfo.pNext = nullptr;
    dsInfo.flags = 0;
    dsInfo.dynamicStateCount = 2;
    dsInfo.pDynamicStates = dynamicStates;

    VkGraphicsPipelineCreateInfo pInfo;

    pInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pInfo.pNext = nullptr;
    pInfo.flags = 0;
    pInfo.stageCount = 2;
    pInfo.pStages = ssInfo;
    pInfo.pVertexInputState = &viInfo;
    pInfo.pInputAssemblyState = &iasInfo;
    pInfo.pTessellationState = nullptr;
    pInfo.pViewportState = &vsInfo;
    pInfo.pRasterizationState = &rsInfo;
    pInfo.pMultisampleState = &msInfo;
    pInfo.pDepthStencilState = &dssInfo;
    pInfo.pColorBlendState = &bsInfo;
    pInfo.pDynamicState = &dsInfo;
    pInfo.layout = info.mPipelineLayout->GetHandle();
    pInfo.renderPass = info.mRenderpass->GetHandle();
    pInfo.subpass = 0;
    pInfo.basePipelineHandle = nullptr;
    pInfo.basePipelineIndex = 0;

    VK(vkCreateGraphicsPipelines(mDevice->GetHandle(), VK_NULL_HANDLE, 1, &pInfo, nullptr, &mPipelineHandle));

}

}