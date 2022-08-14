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

class DescriptorSetLayout {
public:
    DescriptorSetLayout();
    DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    ~DescriptorSetLayout();

    inline VkDescriptorSetLayout GetHandle() const { return DescriptorSetLayoutHandle; }
    inline std::vector<VkDescriptorSetLayoutBinding> GetBindings() const { return Bindings; }
    inline VkDescriptorSetLayoutBinding GetBinding(uint32_t binding) const { return Bindings[binding]; }
private:
    VkDescriptorSetLayout DescriptorSetLayoutHandle;
    std::vector<VkDescriptorSetLayoutBinding> Bindings;
};

class DescriptorSet {
private:
    friend class DescriptorSetLayout;
public:
    DescriptorSet(VkDescriptorSet Handle, DescriptorSetLayout* layout);
    ~DescriptorSet();

    inline VkDescriptorSet GetHandle() const { return DescriptorSetHandle; }
    inline DescriptorSetLayout* GetLayout() const { return Layout; }
private:
    VkDescriptorSet DescriptorSetHandle;
    DescriptorSetLayout* Layout;

};

class DescriptorPool {
public:
    DescriptorPool(uint32_t maxSets);
    ~DescriptorPool();

    DescriptorSet* AllocateDescriptorSet(DescriptorSetLayout* layout);

    inline VkDescriptorPool GetHandle() const { return PoolHandle; }

private:
    uint32_t RemainingSets;
    VkDescriptorPool PoolHandle;
};

}