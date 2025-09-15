#include "common.hpp"
#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

struct frameBufferCreateInfo{
                  VkDevice      device; 
                  VkRenderPass  renderPass;
const std::vector<VkImageView>* imageViews; 
const std::vector<VkImageView>* depthViews;
                  VkExtent2D    extent ; 
};

class FramebufferManager {
public:
FramebufferManager(frameBufferCreateInfo info)
: device(info.device){
framebuffers.resize(info.imageViews->size());
//color + depth를 바탕으로 frameBuffer 할당
for (size_t i = 0; i < info.imageViews->size(); i++) {
std::array<VkImageView,2> attachments = {info.imageViews->at(i), info.depthViews->at(i)};
    VkFramebufferCreateInfo   framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = info.renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments    = attachments.data();
    framebufferInfo.width           = info.extent.width;
    framebufferInfo.height          = info.extent.height;
    framebufferInfo.layers          = 1;
    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

~FramebufferManager() {
for (auto framebuffer : framebuffers) {
    if(framebuffer!= VK_NULL_HANDLE)
    vkDestroyFramebuffer(device, framebuffer, nullptr);
}
}
VkFramebuffer get(uint32_t imageIndex){return framebuffers[imageIndex];}
private:
VkDevice device;
std::vector<VkFramebuffer> framebuffers;
};


#endif