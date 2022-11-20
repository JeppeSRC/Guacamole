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

#include "semaphore.h"

#include <Guacamole/vulkan/device.h>

namespace Guacamole {

Semaphore* Semaphore::Create(Device* device) {
    if (device->GetFeatures() & Device::FeatureTimelineSemaphore) {
        return new SemaphoreTimeline(device);
    }

    GM_ASSERT(false);

    return nullptr;
}

SemaphoreTimeline::SemaphoreTimeline(Device* device) : Semaphore(device), mCounter(0) {
    VkSemaphoreTypeCreateInfoKHR timeline;

    timeline.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO_KHR;
    timeline.pNext = nullptr;
    timeline.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE_KHR;
    timeline.initialValue = 0;

    VkSemaphoreCreateInfo info;

    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.flags = 0;
    info.pNext = &timeline;

    VK(vkCreateSemaphore(device->GetHandle(), &info, nullptr, &mSemaphore));

    mWaitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO_KHR;
    mWaitInfo.flags = 0;
    mWaitInfo.pNext = nullptr;
    mWaitInfo.semaphoreCount = 1;
    mWaitInfo.pSemaphores = &mSemaphore;
}

SemaphoreTimeline::~SemaphoreTimeline() {
    vkDestroySemaphore(mDevice->GetHandle(), mSemaphore, nullptr);
}

void SemaphoreTimeline::Wait() {
    mWaitInfo.pValues = &mCounter;

    VK(vkWaitSemaphores(mDevice->GetHandle(), &mWaitInfo, ~0));
}

}