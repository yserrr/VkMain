#include <vulkan/vulkan.h>
#include <stdexcept>
#ifndef SAMPLER_HPP
#define SAMPLER_HPP
class Sampler {
public:
Sampler(VkDevice device) : device(device), sampler(VK_NULL_HANDLE) {
    createSampler();
}
~Sampler() {
    if (sampler != VK_NULL_HANDLE) {
        vkDestroySampler(device, sampler, nullptr);
    }
}
VkSampler get() const { return sampler; }
private:
VkDevice device;
VkSampler sampler;
    void createSampler() {
        spdlog::info("create sampler");
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter               = VK_FILTER_LINEAR;
        samplerInfo.minFilter               = VK_FILTER_LINEAR;
        samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable        = VK_FALSE;
        samplerInfo.maxAnisotropy           = 16;
        samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp     = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias    = 0.0f;
        samplerInfo.minLod        = 0.0f;
        samplerInfo.maxLod        = VK_LOD_CLAMP_NONE;
        if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sampler!");
        }
    }
};

#endif