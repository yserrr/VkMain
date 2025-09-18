//
#ifndef GRAPHICSPIPELINE_HPP
#define GRAPHICSPIPELINE_HPP
#include <stdexcept>
#include <vector>
#include <common.hpp>
#include <vertex.hpp>
#include <unordered_map>
#include "pipeline_cfg.hpp"
#include <fstream>

struct PipelinePoolCreateInfo{
  VkDevice device                   = VK_NULL_HANDLE;
  VkRenderPass renderPass           = VK_NULL_HANDLE;
  VkDescriptorSetLayout *baseLayout = nullptr;
  uint32_t descriptorLayoutCount    = 1;
};

struct PipelineCreatePass{
  VertexType type                   = VertexType::PUVN;
  VkShaderModule vertShaderModule   = VK_NULL_HANDLE;
  VkShaderModule fragShaderModule   = VK_NULL_HANDLE;
  VkFormat *colorFormats            = nullptr;
  uint32_t colorFormatCount         = 0;
  VkFormat depthFormat              = VK_FORMAT_D32_SFLOAT;
  VkFormat stencilFormat            = VK_FORMAT_UNDEFINED;
  uint32_t layoutCount              = 0;
  VkDescriptorSetLayout *layoutData = nullptr;
  VkPrimitiveTopology topology      = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkCullModeFlags cullMode          = VK_CULL_MODE_NONE;
  VkBool32 depthTestEnable          = VK_TRUE;
  VkBool32 depthWriteEnable         = VK_TRUE;
  VkCompareOp depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
  uint32_t vertexBinding            = 0;
};

///@param flags -> reserved field, but vk1.3 -> don't use, just 0
class PipelinePool{
public:
  PipelinePool(const PipelinePoolCreateInfo &info);
  ~PipelinePool();

  VkPipeline createPipeline(const PipelineCreatePass &pass);
  VkPipeline createPipeline(VertexType type,
                            VkShaderModule vertexModule,
                            VkShaderModule fragModule,
                            VkFormat *colorAttachmentFormats,
                            uint32_t colorAttachmentCount     = 1,
                            uint32_t viewMask                 = 0,
                            VkFormat depthFormat              = VK_FORMAT_D32_SFLOAT,
                            VkFormat stencilAttachment        = VK_FORMAT_UNDEFINED,
                            uint32_t layoutCount              = 1,
                            VkDescriptorSetLayout *layoutData = VK_NULL_HANDLE,
                            VkPrimitiveTopology topology      = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                            VkCullModeFlags cullMode          = VK_CULL_MODE_NONE,
                            VkBool32 depthTestEnable          = VK_TRUE,
                            VkBool32 depthWriteEnable         = VK_TRUE,
                            VkBool32 stencilTestEnable        = VK_TRUE,
                            VkCompareOp depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL,
                            VkRenderPass renderPass           = VK_NULL_HANDLE);

  void createComputePipeline(const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts,
                             VkShaderModule computeShader);

  void createPipeline(VertexType type, VkShaderModule vertexModule, VkShaderModule fragModule, PipelineCreatePass pass);
  VkPipeline getPipeline(PipelineCreatePass program) const;
  VkPipelineLayout getBaseLayout() const;

private:
  void buildFragmentPipeline(VkPipelineShaderStageCreateInfo &shaderStateCi, VkShaderModule fragModule);
  void buildVertexPipeline(VkPipelineShaderStageCreateInfo &shaderStateCi, VkShaderModule vertexModule);

  void buildVertexDescriptor(VertexType type,
                             VkVertexInputBindingDescription &vertexBindingDesc,
                             std::vector<VkVertexInputAttributeDescription> &vertexAttributeDescriptions,
                             VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
                             uint32_t vertexBinding = 0);

  void buildVertexAllDescriptor(VkVertexInputBindingDescription &vertexBindingDesc,
                                std::vector<VkVertexInputAttributeDescription> &vertexAttributeDescriptions,
                                VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
                                uint32_t vertexBinding = 0);

  void buildDepthStencilPipeline(VkPipelineDepthStencilStateCreateInfo &depthStencilCi,
                                 VkBool32 depthTestEnable   = VK_TRUE,
                                 VkBool32 stencilTestEnable = VK_FALSE,
                                 VkBool32 depthWriteEnable  = VK_TRUE,
                                 VkCompareOp depthCompareOp =
                                 VK_COMPARE_OP_LESS_OR_EQUAL);

  void buildAssemblyPipeline(VkPipelineInputAssemblyStateCreateInfo &inputAssembly,
                             VkPrimitiveTopology topology                  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                             VkBool32 primitiveRestartEnable               = VK_FALSE,
                             VkPipelineInputAssemblyStateCreateFlags flags = 0);

  void buildDynamicPipelineDyscriptor(VkPipelineDynamicStateCreateInfo &dynamicStateCi,
                                      std::vector<VkDynamicState> &dynamicStates,
                                      VkPipelineViewportStateCreateInfo &viewportStateCi,
                                      uint32_t viewCount                      = 1,
                                      VkBool32 dynamicDepthTestEnable         = VK_FALSE,
                                      VkBool32 dynamicStencilTestEnable       = VK_FALSE,
                                      VkBool32 dynamicStateDepthCompare       = VK_FALSE,
                                      VkBool32 dynamicStateVetexStride        = VK_FALSE,
                                      VkPipelineDynamicStateCreateFlags flags = 0);

  void buildRasterizationPipeline(VkPipelineRasterizationStateCreateInfo &rasterizeCi,
                                  VkCullModeFlags cullMode                      = VK_CULL_MODE_NONE,
                                  VkPolygonMode mode                            = VK_POLYGON_MODE_FILL,
                                  VkFrontFace front                             = VK_FRONT_FACE_CLOCKWISE,
                                  VkPipelineRasterizationStateCreateFlags flags = 0);

  void buildMultiSamplingPipeline(VkPipelineMultisampleStateCreateInfo &multiSamplingCi,
                                  VkSampleCountFlagBits samples               = VK_SAMPLE_COUNT_1_BIT,
                                  VkPipelineMultisampleStateCreateFlags flags = 0);

  void buildColorBlendingAttachment(VkPipelineColorBlendAttachmentState &colorBlendAttachment,
                                    uint32_t             = COLOR_FLAG_ALL,
                                    VkBool32 blendEnable = VK_FALSE);
  void buildColorBlendingPipeline(VkPipelineColorBlendStateCreateInfo &colorBlendingCi,
                                  VkPipelineColorBlendAttachmentState attachment,
                                  uint32_t attachmentCount = 1);

  void buildDynamicRenderingPipeline(VkPipelineRenderingCreateInfo &dynamicRendering,
                                     VkFormat *colorAttachmentFormats,
                                     uint32_t colorAttachmentCount = 1,
                                     uint32_t viewMask             = 0,
                                     VkFormat depthFormat          = VK_FORMAT_D32_SFLOAT,
                                     VkFormat stencilAttachment    = VK_FORMAT_UNDEFINED
    );

  VkPipelineLayout createPipelineLayout(VkDescriptorSetLayout *descriptorLayoutData,
                                        uint32_t layoutCount);
  std::vector<uint8_t> loadPipelineCache(const std::string &filename);

private:
  VkDevice device_;
  VkRenderPass renderPass_;
  VkPipelineLayout basePipelineLayout_;
  VkPipelineLayout computePipelineLayout_;

  VkPipelineCache oldPipelineCache_;
  std::unordered_map<PipelineProgram, VkPipeline, PipelineHash> pipelineHash_{};
};

#endif