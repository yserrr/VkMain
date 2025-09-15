//
// Created by ljh on 25. 9. 15..
//

#ifndef MYPROJECT_PIPELINE_CFG_HPP
#define MYPROJECT_PIPELINE_CFG_HPP


#ifndef MYPROJECT_PIPELINE_CFG_HPP
#define MYPROJECT_PIPELINE_CFG_HPP
#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>

enum class PipelineUsageFlag:uint32_t{
  POLYGON_FILL  = 0x00000001,
  POLYGON_LINE  = 0x00000002,
  POLYGON_POINT = 0x00000004,

  CULL_FRONT = 0x00000008,
  CULL_BACK  = 0x00000010,
  CULL_NULL  = 0x00000026,

  DEPTH_TEST   = 0x00000040,
  STENCIL_TEST = 0x00000080,

  POINT_LIST = 0x00000100,
  LINE_LIST  = 0x00000200,
  LINE_STRIP = 0x00000400,

  TRIANGLE_STRIP = 0x00000800,
  TRIANGLE_LIST  = 0x00001000,

  CW      = 0x00002000,
  CCW     = 0x00004000,
  COMPUTE = 0x00008000,
};

struct PipelineProgram{
  VkPipelineLayout layout         = VK_NULL_HANDLE;
  VkShaderModule vertShaderModule = VK_NULL_HANDLE;
  VkShaderModule fragShaderModule = VK_NULL_HANDLE;
  VkPrimitiveTopology topology    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkCullModeFlags cullMode        = VK_CULL_MODE_NONE;
  VkBool32 depthTestEnable        = VK_TRUE;
  VkBool32 depthWriteEnable       = VK_TRUE;
  VkCompareOp depthCompareOp      = VK_COMPARE_OP_LESS_OR_EQUAL;
  bool operator==(const PipelineProgram &other) const
  {
    return layout == other.layout &&
           vertShaderModule == other.vertShaderModule &&
           fragShaderModule == other.fragShaderModule &&
           topology == other.topology &&
           cullMode == other.cullMode &&
           depthTestEnable == other.depthTestEnable;
  }
};

struct PipelineHash{
  std::size_t operator()(const PipelineProgram &program) const
  {
    return std::hash<VkShaderModule>()(program.vertShaderModule) ^
           (std::hash<VkShaderModule>()(program.fragShaderModule) << 1);
  }
};

constexpr uint32_t COLOR_FLAG_ALL = VK_COLOR_COMPONENT_R_BIT |
                                    VK_COLOR_COMPONENT_G_BIT |
                                    VK_COLOR_COMPONENT_B_BIT |
                                    VK_COLOR_COMPONENT_A_BIT;

struct PipelineCi{
  VkPipelineShaderStageCreateInfo vertShaderStageInfo_{};
  VkPipelineShaderStageCreateInfo fragShaderStageInfo_{};
  VkVertexInputBindingDescription vertexBindingDesc_{};
  std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions_{};
  std::vector<VkDynamicState> dynamicStates{};
  VkPipelineDynamicStateCreateInfo dynamicStateCi{};
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  VkPipelineVertexInputStateCreateInfo vertexInputInfo_{};
  VkPipelineDepthStencilStateCreateInfo depthStencilCi{};
  VkPipelineShaderStageCreateInfo shaderStagesCi[2] {};
  VkPipelineRasterizationStateCreateInfo rasterizeCi{};
  VkPipelineColorBlendStateCreateInfo colorBlendingCi{};
  VkPipelineViewportStateCreateInfo viewportStateCi{};
  VkPipelineMultisampleStateCreateInfo multiSamplingCi{};
  VkPipelineLayoutCreateInfo pipelineLayoutCi{};
  VkPipelineRenderingCreateInfo dynamicRenderingInfo{};
};

struct PipelineUsage{
  uint32_t FILL_TRIANGLE_LIST_NULL =
  static_cast<uint32_t>(PipelineUsageFlag::POLYGON_FILL) |
  static_cast<uint32_t>(PipelineUsageFlag::TRIANGLE_LIST) |
  static_cast<uint32_t>(PipelineUsageFlag::CULL_NULL);

  // FILL_TRIGANGLE_LIST_NULL
  uint32_t FILL_LINE_LIST_NULL =
  static_cast<uint32_t>(PipelineUsageFlag::POLYGON_FILL) |
  static_cast<uint32_t>(PipelineUsageFlag::LINE_LIST) |
  static_cast<uint32_t>(PipelineUsageFlag::CULL_NULL);

  uint32_t FILL_POINT_LIST_NULL =
  static_cast<uint32_t>(PipelineUsageFlag::POLYGON_FILL) |
  static_cast<uint32_t>(PipelineUsageFlag::POINT_LIST) |
  static_cast<uint32_t>(PipelineUsageFlag::CULL_NULL);

  uint32_t LINE_TRIANGLE_LIST_NULL =
  static_cast<uint32_t>(PipelineUsageFlag::POLYGON_LINE) |
  static_cast<uint32_t>(PipelineUsageFlag::TRIANGLE_LIST) |
  static_cast<uint32_t>(PipelineUsageFlag::CULL_NULL);

  uint32_t LINE_LIST_LIST_NULL =
  static_cast<uint32_t>(PipelineUsageFlag::POLYGON_LINE) |
  static_cast<uint32_t>(PipelineUsageFlag::LINE_LIST) |
  static_cast<uint32_t>(PipelineUsageFlag::CULL_NULL);

  uint32_t LINE_POINT_LIST_NULL =
  static_cast<uint32_t>(PipelineUsageFlag::POLYGON_LINE) |
  static_cast<uint32_t>(PipelineUsageFlag::POINT_LIST) |
  static_cast<uint32_t>(PipelineUsageFlag::CULL_NULL);

  uint32_t POINT_TRIANGLE_LIST_NULL =
  static_cast<uint32_t>(PipelineUsageFlag::POLYGON_POINT) |
  static_cast<uint32_t>(PipelineUsageFlag::TRIANGLE_LIST) |
  static_cast<uint32_t>(PipelineUsageFlag::CULL_NULL);

  uint32_t POINT_LINE_LIST_NULL =
  static_cast<uint32_t>(PipelineUsageFlag::POLYGON_POINT) |
  static_cast<uint32_t>(PipelineUsageFlag::LINE_LIST) |
  static_cast<uint32_t>(PipelineUsageFlag::CULL_NULL);

  uint32_t POINT_POINT_LIST_NULL =
  static_cast<uint32_t>(PipelineUsageFlag::POLYGON_POINT) |
  static_cast<uint32_t>(PipelineUsageFlag::POINT_LIST) |
  static_cast<uint32_t>(PipelineUsageFlag::CULL_NULL);
};

#endif //MYPROJECT_PIPELINE_CFG_HPP

#endif //MYPROJECT_PIPELINE_CFG_HPP