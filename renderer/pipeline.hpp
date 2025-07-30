#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "vertex.hpp"
//api: 규격 , 명세서 
#ifndef GRAPHICSPIPELINE_HPP
#define GRAPHICSPIPELINE_HPP 

struct pipelineCreateInfo{ 
      VkDevice                             device;
      VkExtent2D                           extent; 
      VkRenderPass                         renderPass;
      VkShaderModule                       vertShaderModule; 
      VkShaderModule                       fragShaderModule;
const std::vector<VkDescriptorSetLayout>*  descriptorSetLayouts;
};

class GraphicsPipeline {
public:
GraphicsPipeline(const pipelineCreateInfo& info)
: device(info.device)
{ 
// 1. 셰이더 스테이지
VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
vertShaderStageInfo.module = info.vertShaderModule;
vertShaderStageInfo.pName  = "main";
//shader binding
VkVertexInputBindingDescription bindingDescription{};
bindingDescription.binding   = 0;
bindingDescription.stride    = sizeof(Vertex);
bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

std::array<VkVertexInputAttributeDescription, 8> attributeDescriptions{};
attributeDescriptions[0].binding  = 0;
attributeDescriptions[0].location = 0;
attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;  // position (vec3)
attributeDescriptions[0].offset   = offsetof(Vertex, position);

attributeDescriptions[1].binding  = 0;
attributeDescriptions[1].location = 1;
attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;  // normal (vec3)
attributeDescriptions[1].offset   = offsetof(Vertex, normal);

attributeDescriptions[2].binding  = 0;
attributeDescriptions[2].location = 2;
attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;     // uv (vec2)
attributeDescriptions[2].offset   = offsetof(Vertex, uv);

attributeDescriptions[3].binding  = 0;
attributeDescriptions[3].location = 3;
attributeDescriptions[3].format   = VK_FORMAT_R32G32B32_SFLOAT;  // tangent (vec3)
attributeDescriptions[3].offset   = offsetof(Vertex, tangent);

attributeDescriptions[4].binding  = 0;
attributeDescriptions[4].location = 4;
attributeDescriptions[4].format   = VK_FORMAT_R32G32B32_SFLOAT;  // bitangent (vec3)
attributeDescriptions[4].offset   = offsetof(Vertex, bitangent);

attributeDescriptions[5].binding  = 0;
attributeDescriptions[5].location = 5;
attributeDescriptions[5].format   = VK_FORMAT_R32G32B32A32_SFLOAT;  // color (vec4)
attributeDescriptions[5].offset   = offsetof(Vertex, color);

attributeDescriptions[6].binding  = 0;
attributeDescriptions[6].location = 6;
attributeDescriptions[6].format   = VK_FORMAT_R32G32B32A32_SINT;  // boneIndices (ivec4)
attributeDescriptions[6].offset   = offsetof(Vertex, boneIndices);

attributeDescriptions[7].binding  = 0;
attributeDescriptions[7].location = 7;
attributeDescriptions[7].format   = VK_FORMAT_R32G32B32A32_SFLOAT;  // boneWeights (vec4)
attributeDescriptions[7].offset   = offsetof(Vertex, boneWeights);

VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
fragShaderStageInfo.module = info.fragShaderModule;
fragShaderStageInfo.pName  = "main";
VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
// 2. Vertex Input
VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
vertexInputInfo.vertexBindingDescriptionCount   = 1;
vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();
//depth information
VkPipelineDepthStencilStateCreateInfo depthStencil{};
depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
depthStencil.depthTestEnable       = VK_TRUE;               // 깊이 테스트 사용
depthStencil.depthWriteEnable      = VK_TRUE;              // 깊이 값 기록 여부
depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;     // 깊이 비교 방식
depthStencil.depthBoundsTestEnable = VK_FALSE;
depthStencil.stencilTestEnable     = VK_FALSE;
// 3. Input Assembly
VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
inputAssembly.primitiveRestartEnable = VK_FALSE;
// 4. Viewport/Scissor
VkViewport viewport{};
viewport.x        = 0.0f;
viewport.y        = 0.0f;
viewport.width    = (float) info.extent.width;
viewport.height   = (float)info.extent.height;
viewport.minDepth = 0.0f;
viewport.maxDepth = 1.0f;
VkRect2D scissor{};
scissor.offset = {0, 0};
scissor.extent = info.extent;

VkPipelineViewportStateCreateInfo viewportState{};
viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
viewportState.viewportCount = 1;
viewportState.pViewports    = &viewport;
viewportState.scissorCount  = 1;
viewportState.pScissors     = &scissor;

// 5. Rasterizer
VkPipelineRasterizationStateCreateInfo rasterizer{};
rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
rasterizer.depthClampEnable        = VK_FALSE;
rasterizer.rasterizerDiscardEnable = VK_FALSE;
rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
rasterizer.lineWidth               = 1.0f;
rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
rasterizer.depthBiasEnable         = VK_FALSE;
// 6. Multisampling (비활성화)
VkPipelineMultisampleStateCreateInfo multisampling{};
multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
// 7. Color Blend
VkPipelineColorBlendAttachmentState colorBlendAttachment{};
colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                      VK_COLOR_COMPONENT_G_BIT |
                                      VK_COLOR_COMPONENT_B_BIT |
                                      VK_COLOR_COMPONENT_A_BIT;
colorBlendAttachment.blendEnable = VK_FALSE;
VkPipelineColorBlendStateCreateInfo colorBlending{};
colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
colorBlending.attachmentCount = 1;
colorBlending.pAttachments    = &colorBlendAttachment;
VkDynamicState dynamicStates[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
};
VkPipelineDynamicStateCreateInfo dynamicStateInfo{} ;
dynamicStateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO; 
dynamicStateInfo.dynamicStateCount = 2;
dynamicStateInfo.pDynamicStates    = dynamicStates;
//descriptorPool Manager 에서 관리
VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
pipelineLayoutInfo.setLayoutCount = info.descriptorSetLayouts->size();
pipelineLayoutInfo.pSetLayouts    = info.descriptorSetLayouts->data(); // <- 여기 필수
if (vkCreatePipelineLayout(info.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
}
// 9. 파이프라인 생성
VkGraphicsPipelineCreateInfo pipelineInfo{};
pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
pipelineInfo.stageCount          = 2;
pipelineInfo.pStages             = shaderStages;
pipelineInfo.pVertexInputState   = &vertexInputInfo;
pipelineInfo.pInputAssemblyState = &inputAssembly;
pipelineInfo.pViewportState      = &viewportState;
pipelineInfo.pRasterizationState = &rasterizer;
pipelineInfo.pMultisampleState   = &multisampling;
pipelineInfo.pColorBlendState    = &colorBlending;
pipelineInfo.pDepthStencilState  = &depthStencil; //depth append
pipelineInfo.pDynamicState       = &dynamicStateInfo;
pipelineInfo.layout              = pipelineLayout;
pipelineInfo.renderPass          = info.renderPass;
pipelineInfo.subpass             = 0;
if (vkCreateGraphicsPipelines(info.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
}
}

~GraphicsPipeline() {
vkDestroyPipeline(device, graphicsPipeline, nullptr);
vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
spdlog::info("destory pipeline");
}
VkPipeline       get()       const { return graphicsPipeline; }
VkPipelineLayout getLayout() const { return pipelineLayout; }

private:
VkDevice         device;
VkPipeline       graphicsPipeline;
VkPipelineLayout pipelineLayout;
};

#endif