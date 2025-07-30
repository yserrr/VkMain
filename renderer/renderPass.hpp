#include "common.hpp"
//shader render pass 관리 주체 
//shader attach 
//depth  buffer attach
#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP


struct renderPassCreateInfo{ 
VkDevice  device;
VkFormat  colorFormat;
};
class RenderPass {
public:
RenderPass(renderPassCreateInfo& info)
:device     (info.device),
 colorFormat(info.colorFormat), 
 renderPass(VK_NULL_HANDLE)
{
// 1. Color Attachment
VkAttachmentDescription colorAttachment{};
colorAttachment.format         = colorFormat;
colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//UI SubPass
VkAttachmentDescription UI{};
UI.format         = colorFormat;
UI.samples        = VK_SAMPLE_COUNT_1_BIT;
UI.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;  // Clear 화면
UI.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
UI.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
UI.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
UI.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
UI.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//depth attchment 
VkAttachmentDescription depthAttachment{};
depthAttachment.format         = VK_FORMAT_D32_SFLOAT;
depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
// Attachment array (color, depth)
std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

// Attachment references
VkAttachmentReference colorAttachmentRef{};
colorAttachmentRef.attachment = 0;
colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

VkAttachmentReference depthAttachmentRef{};
depthAttachmentRef.attachment = 1;
depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

// UI Subpass color ref (같은 attachment 0)
VkAttachmentReference uiColorAttachmentRef{};
uiColorAttachmentRef.attachment = 0;
uiColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

// Subpass 0 : Scene
VkSubpassDescription sceneSubpass{};
sceneSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
sceneSubpass.colorAttachmentCount = 1;
sceneSubpass.pColorAttachments = &colorAttachmentRef;
sceneSubpass.pDepthStencilAttachment = &depthAttachmentRef;

// Subpass 1 : UI (depth attachment 없음)
VkSubpassDescription uiSubpass{};
uiSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
uiSubpass.colorAttachmentCount = 1;
uiSubpass.pColorAttachments = &uiColorAttachmentRef;
uiSubpass.pDepthStencilAttachment = nullptr;

// Subpass dependencies
std::array<VkSubpassDependency, 2> dependencies{};

// 외부 → Scene Subpass
dependencies[0].srcSubpass    =   VK_SUBPASS_EXTERNAL;
dependencies[0].dstSubpass    =   0;
dependencies[0].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
dependencies[0].dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
dependencies[0].srcAccessMask = 0;
dependencies[0].dstAccessMask =  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //depth 단계 끝나야함
dependencies[0].dependencyFlags = 0;

// Scene Subpass → UI Subpass
dependencies[1].srcSubpass = 0;
dependencies[1].dstSubpass = 1;
dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
dependencies[1].dependencyFlags = 0;

// RenderPass CreateInfo
VkRenderPassCreateInfo renderPassInfo{};
renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
renderPassInfo.pAttachments = attachments.data();
renderPassInfo.subpassCount = 2;
std::array<VkSubpassDescription,2> subpasses = {sceneSubpass, uiSubpass};
renderPassInfo.pSubpasses = subpasses.data();
renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
renderPassInfo.pDependencies = dependencies.data();
if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
}
}
~RenderPass() {
if (renderPass != VK_NULL_HANDLE) {
    vkDestroyRenderPass(device, renderPass, nullptr);
}
}
VkRenderPass get() const { return renderPass; }

private:
VkDevice      device;
VkRenderPass  renderPass;
VkFormat      colorFormat;
};
 

#endif