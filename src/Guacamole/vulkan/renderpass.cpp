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
#include "renderpass.h"

#include "swapchain.h"

namespace Guacamole {

Renderpass::~Renderpass() {
    vkDestroyRenderPass(mDevice->GetHandle(), mRenderpassHandle, nullptr);
}

void Renderpass::Create(VkRenderPassCreateInfo* rInfo) {
    GM_ASSERT(rInfo)

    VK(vkCreateRenderPass(mDevice->GetHandle(), rInfo, nullptr, &mRenderpassHandle));
}

BasicRenderpass::BasicRenderpass(Swapchain* swapchain, Device* device) : Renderpass(device, swapchain) {
    VkRenderPassCreateInfo info;

    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    VkAttachmentDescription attchments[2];

    attchments[0].flags = 0;
    attchments[0].format = swapchain->GetFormat();
    attchments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attchments[1].flags = 0;
    attchments[1].format = VK_FORMAT_D16_UNORM;
    attchments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    info.attachmentCount = 2;
    info.pAttachments = attchments;

    VkAttachmentReference colorRef;

    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthRef;

    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subDesc;

    subDesc.flags = 0;
    subDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subDesc.inputAttachmentCount = 0;
    subDesc.pInputAttachments = nullptr;
    subDesc.colorAttachmentCount = 1;
    subDesc.pColorAttachments = &colorRef;
    subDesc.pResolveAttachments = nullptr;
    subDesc.pDepthStencilAttachment = &depthRef;
    subDesc.preserveAttachmentCount = 0;
    subDesc.pPreserveAttachments = nullptr;

    info.subpassCount = 1;
    info.pSubpasses = &subDesc;
    info.dependencyCount = 0;
    info.pDependencies = nullptr;

    Create(&info);

    std::vector<VkImageView> imageViews = swapchain->GetImageViews();

    mFramebuffers.reserve(imageViews.size());

    VkExtent2D extent = swapchain->GetExtent();

    for (uint32_t i = 0; i < imageViews.size(); i++) {
        mFramebuffers.emplace_back(mDevice, extent.width, extent.height, mRenderpassHandle, imageViews[i], VK_FORMAT_D16_UNORM);
        swapchain->AddFramebuffer(i, &mFramebuffers[i]);
    }

    mBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    mBeginInfo.pNext = nullptr;
    mBeginInfo.renderPass = mRenderpassHandle;
    mBeginInfo.renderArea.offset.x = 0;
    mBeginInfo.renderArea.offset.y = 0;
}

BasicRenderpass::~BasicRenderpass() {
    for (uint32_t i = 0; i < mFramebuffers.size(); i++) 
        mSwapchain->RemoveFramebuffer(i, &mFramebuffers[i]);
}

void BasicRenderpass::Begin(const CommandBuffer* cmd) {
    VkClearValue clear[2]{};

    clear[1].depthStencil.depth = 1.0f;

    mBeginInfo.renderArea.extent = mSwapchain->GetExtent();
    mBeginInfo.framebuffer = GetFramebufferHandle(mSwapchain->GetCurrentImageIndex());
    mBeginInfo.clearValueCount = 2;
    mBeginInfo.pClearValues = clear;
    
    
    vkCmdBeginRenderPass(cmd->GetHandle(), &mBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void BasicRenderpass::End(const CommandBuffer* cmd) {
    vkCmdEndRenderPass(cmd->GetHandle());
}

VkFramebuffer BasicRenderpass::GetFramebufferHandle(uint32_t index) const {
    return mFramebuffers[index].GetHandle();
}


}