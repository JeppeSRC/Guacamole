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

#include "context.h"
#include "swapchain.h"

namespace Guacamole {

Renderpass::~Renderpass() {
    vkDestroyRenderPass(Context::GetDeviceHandle(), mRenderpassHandle, nullptr);
}

void Renderpass::Create(VkRenderPassCreateInfo* rInfo) {
    GM_ASSERT(rInfo)

    VK(vkCreateRenderPass(Context::GetDeviceHandle(), rInfo, nullptr, &mRenderpassHandle));
}

BasicRenderpass::BasicRenderpass() {
    VkRenderPassCreateInfo info;

    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    VkAttachmentDescription colorDesc;

    colorDesc.flags = 0;
    colorDesc.format = Swapchain::GetFormat();
    colorDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    colorDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    info.attachmentCount = 1;
    info.pAttachments = &colorDesc;

    VkAttachmentReference colorRef;

    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subDesc;

    subDesc.flags = 0;
    subDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subDesc.inputAttachmentCount = 0;
    subDesc.pInputAttachments = nullptr;
    subDesc.colorAttachmentCount = 1;
    subDesc.pColorAttachments = &colorRef;
    subDesc.pResolveAttachments = nullptr;
    subDesc.pDepthStencilAttachment = nullptr;
    subDesc.preserveAttachmentCount = 0;
    subDesc.pPreserveAttachments = nullptr;

    info.subpassCount = 1;
    info.pSubpasses = &subDesc;
    info.dependencyCount = 0;
    info.pDependencies = nullptr;

    Create(&info);

    VkFramebufferCreateInfo fbInfo;

    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.pNext = nullptr;
    fbInfo.flags = 0;
    fbInfo.renderPass = mRenderpassHandle;
    fbInfo.attachmentCount = 1;
    //fbInfo.pAttachments = nullptr;
    fbInfo.width = Swapchain::GetExtent().width;
    fbInfo.height = Swapchain::GetExtent().height;
    fbInfo.layers = 1;

    std::vector<VkImageView> views = Swapchain::GetImageViews();

    for (VkImageView view : views) {
        fbInfo.pAttachments = &view;

        VkFramebuffer fb;
        VK(vkCreateFramebuffer(Context::GetDeviceHandle(), &fbInfo, nullptr, &fb));

        mFramebuffers.push_back(fb);
    }

    mBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    mBeginInfo.pNext = nullptr;
    mBeginInfo.renderPass = mRenderpassHandle;
    mBeginInfo.renderArea.extent = Swapchain::GetExtent();
    mBeginInfo.renderArea.offset.x = 0;
    mBeginInfo.renderArea.offset.y = 0;
}

BasicRenderpass::~BasicRenderpass() {
    for (VkFramebuffer fb : mFramebuffers) {
        vkDestroyFramebuffer(Context::GetDeviceHandle(), fb, nullptr);
    }
}

void BasicRenderpass::Begin(CommandBuffer* cmd) {
    VkClearValue clear = {};

    mBeginInfo.framebuffer = GetFramebufferHandle(Swapchain::GetCurrentImageIndex());
    mBeginInfo.clearValueCount = 1;
    mBeginInfo.pClearValues = &clear;
    
    vkCmdBeginRenderPass(cmd->GetHandle(), &mBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void BasicRenderpass::End(CommandBuffer* cmd) {
    vkCmdEndRenderPass(cmd->GetHandle());
}

VkFramebuffer BasicRenderpass::GetFramebufferHandle(uint32_t index) const {
    return mFramebuffers[index];
}

}