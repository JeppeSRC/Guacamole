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

#include "sampler.h"

#include <Guacamole/vulkan/device.h>

namespace Guacamole {

Sampler::Sampler(Device* device) 
    : mSamplerHandle(VK_NULL_HANDLE), mDevice(device) {}


void Sampler::Create(const VkSamplerCreateInfo& info) {
    VK(vkCreateSampler(mDevice->GetHandle(), &info, nullptr, &mSamplerHandle));
}

Sampler::~Sampler() {
    vkDestroySampler(mDevice->GetHandle(), mSamplerHandle, nullptr);
}


BasicSampler::BasicSampler(Device* device, VkFilter magFilter, VkFilter minFilter, VkSamplerAddressMode addressMode) : Sampler(device) {
    VkSamplerCreateInfo sInfo;

    sInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sInfo.pNext = nullptr;
    sInfo.flags = 0;
    sInfo.magFilter = magFilter;
    sInfo.minFilter = minFilter;
    sInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sInfo.addressModeU = addressMode;
    sInfo.addressModeV = addressMode;
    sInfo.addressModeW = addressMode;
    sInfo.mipLodBias = 0;
    sInfo.anisotropyEnable = device->GetParent()->IsFeatureSupported(Device::FeatureAnisotropicSampling);
    sInfo.maxAnisotropy = 16;
    sInfo.compareEnable = false;
    sInfo.compareOp = VK_COMPARE_OP_EQUAL;
    sInfo.minLod = 0;
    sInfo.maxLod = 0;
    sInfo.unnormalizedCoordinates = false;

    Create(sInfo);
}


}