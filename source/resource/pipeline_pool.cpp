
#include<constant.hpp>
#include <pipeline_pool.hpp>
PipelinePool::PipelinePool(
    const PipelinePoolCreateInfo &info
  ) :
  device_(info.device),
  renderPass_(info.renderPass)
{
  std::vector<uint8_t> oldCacheData =
  loadPipelineCache("check this");

  VkPipelineCacheCreateInfo cacheInfo{};
  cacheInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  cacheInfo.initialDataSize = oldCacheData.size();
  cacheInfo.pInitialData    = oldCacheData.empty() ? nullptr : oldCacheData.data();
  vkCreatePipelineCache(device_, &cacheInfo, nullptr, &oldPipelineCache_);

  basePipelineLayout_ = createPipelineLayout(info.baseLayout, info.descriptorLayoutCount);
}

PipelinePool::~PipelinePool()
{
  size_t dataSize = 0;
  vkGetPipelineCacheData(device_, oldPipelineCache_, &dataSize, nullptr);
  std::vector<uint8_t> cacheData(dataSize);
  vkGetPipelineCacheData(device_, oldPipelineCache_, &dataSize, cacheData.data());
  std::ofstream outFile("/home/ljh/바탕화면/vk/source/backend/cmake-build-debug/pipeline_cache/pipeline_cache.bin",
                        std::ios::binary);
  outFile.write(reinterpret_cast<char *>(cacheData.data()), cacheData.size());

  for (auto pipeline: pipelineHash_)
  {
    if (pipeline.second != VK_NULL_HANDLE)
    {
      vkDestroyPipeline(device_, pipeline.second, nullptr);
    }
  }
  vkDestroyPipelineLayout(device_, basePipelineLayout_, nullptr);
  spdlog::info("destroy pipeline");
}

VkPipeline PipelinePool::createPipeline(const PipelineCreatePass &pass)
{
  if (getPipeline(pass) == VK_NULL_HANDLE)
  {
    return createPipeline(pass.type,
                          pass.vertShaderModule,
                          pass.fragShaderModule,
                          pass.colorFormats,
                          pass.colorFormatCount
      );
  }
  return getPipeline(pass);
}

void PipelinePool::createComputePipeline(
    const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts,
    VkShaderModule computeShader
  )
{
  //push constant setting
  VkPushConstantRange pushConstant{};
  pushConstant.offset     = 0;
  //pushConstant.size       = sizeof(gpu::constant);
  pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  //if need -> create and layout setting
  VkPipelineLayoutCreateInfo computeLayout{};
  computeLayout.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  computeLayout.pNext                  = nullptr;
  computeLayout.pSetLayouts            = descriptorSetLayouts.data();
  computeLayout.setLayoutCount         = descriptorSetLayouts.size();
  computeLayout.pPushConstantRanges    = &pushConstant;
  computeLayout.pushConstantRangeCount = 1;
  computeLayout.pPushConstantRanges    = &pushConstant;
  computeLayout.pushConstantRangeCount = 1;

  if (vkCreatePipelineLayout(device_, &computeLayout, nullptr, &computePipelineLayout_) != VK_SUCCESS)
  {
    throw std::runtime_error("devicecreate pipeline layout!");
  }
  VkPipelineShaderStageCreateInfo shaderStageInfo{};
  shaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageInfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
  shaderStageInfo.module = computeShader;
  shaderStageInfo.pName  = "main";

  VkComputePipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipelineInfo.stage              = shaderStageInfo;
  pipelineInfo.layout             = computePipelineLayout_;
  pipelineInfo.flags              = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex  = -1;
}

VkPipeline PipelinePool::getPipeline(
    PipelineCreatePass program
  ) const
{
  auto it = pipelineHash_.find(program);
  if (it != pipelineHash_.end())
    return it->second;
  return VK_NULL_HANDLE;
}

VkPipelineLayout PipelinePool::getBaseLayout() const
{
  return basePipelineLayout_;
}

void PipelinePool::buildVertexDescriptor(VertexType type,
                                         VkVertexInputBindingDescription &vertexBindingDesc,
                                         std::vector<VkVertexInputAttributeDescription> &vertexAttributeDescriptions,
                                         VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
                                         uint32_t vertexBinding)
{
  switch (type)
  {
    case (VertexType::ALL):
    {
      return buildVertexAllDescriptor(vertexBindingDesc,
                                      vertexAttributeDescriptions,
                                      vertexInputInfo);
      break;
    }
    case (VertexType::PC):
    {
      vertexBindingDesc.binding   = vertexBinding;
      vertexBindingDesc.stride    = sizeof(VertexPC);
      vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      vertexAttributeDescriptions.resize(2);
      vertexAttributeDescriptions[0].binding  = vertexBinding;
      vertexAttributeDescriptions[0].location = 0;
      vertexAttributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
      vertexAttributeDescriptions[0].offset   = offsetof(VertexPC, position);

      vertexAttributeDescriptions[1].binding  = vertexBinding;
      vertexAttributeDescriptions[1].location = 1;
      vertexAttributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
      vertexAttributeDescriptions[1].offset   = offsetof(VertexPC, color);
      break;
    }
    case (VertexType::PUVN):
    {
      vertexBindingDesc.binding   = vertexBinding;
      vertexBindingDesc.stride    = sizeof(VertexPUVN);
      vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      vertexAttributeDescriptions.resize(3);
      vertexAttributeDescriptions[0].binding  = vertexBinding;
      vertexAttributeDescriptions[0].location = 0;
      vertexAttributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
      vertexAttributeDescriptions[0].offset   = offsetof(VertexPUVN, position);

      vertexAttributeDescriptions[1].binding  = vertexBinding;
      vertexAttributeDescriptions[1].location = 1;
      vertexAttributeDescriptions[1].format   = VK_FORMAT_R32G32_SFLOAT;
      vertexAttributeDescriptions[1].offset   = offsetof(VertexPUVN, uv);

      vertexAttributeDescriptions[2].binding  = vertexBinding;
      vertexAttributeDescriptions[2].location = 2;
      vertexAttributeDescriptions[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
      vertexAttributeDescriptions[2].offset   = offsetof(VertexPUVN, normal);
      break;
    }
    case (VertexType::PUVNTC):
    {
      vertexBindingDesc.binding   = vertexBinding;
      vertexBindingDesc.stride    = sizeof(VertexPUVNTC);
      vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      vertexAttributeDescriptions.resize(5);

      vertexAttributeDescriptions[0].binding  = vertexBinding;
      vertexAttributeDescriptions[0].location = 0;
      vertexAttributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
      vertexAttributeDescriptions[0].offset   = offsetof(VertexPUVNTC, position);

      vertexAttributeDescriptions[1].binding  = vertexBinding;
      vertexAttributeDescriptions[1].location = 1;
      vertexAttributeDescriptions[1].format   = VK_FORMAT_R32G32_SFLOAT;
      vertexAttributeDescriptions[1].offset   = offsetof(VertexPUVNTC, uv);

      vertexAttributeDescriptions[2].binding  = vertexBinding;
      vertexAttributeDescriptions[2].location = 2;
      vertexAttributeDescriptions[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
      vertexAttributeDescriptions[2].offset   = offsetof(VertexPUVNTC, normal);

      vertexAttributeDescriptions[3].binding  = vertexBinding;
      vertexAttributeDescriptions[3].location = 3;
      vertexAttributeDescriptions[3].format   = VK_FORMAT_R32G32B32_SFLOAT;
      vertexAttributeDescriptions[3].offset   = offsetof(VertexPUVNTC, tangent);

      vertexAttributeDescriptions[4].binding  = vertexBinding;
      vertexAttributeDescriptions[4].location = 4;
      vertexAttributeDescriptions[4].format   = VK_FORMAT_R32G32B32_SFLOAT;
      vertexAttributeDescriptions[4].offset   = offsetof(VertexPUVNTC, color);
      break;
    }
    default:
    {
      buildVertexAllDescriptor(vertexBindingDesc, vertexAttributeDescriptions, vertexInputInfo);
      break;
    }
  }
  vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount   = 1;
  vertexInputInfo.pVertexBindingDescriptions      = &vertexBindingDesc;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions    = vertexAttributeDescriptions.data();
}


void PipelinePool::buildVertexAllDescriptor(VkVertexInputBindingDescription &vertexBindingDesc,
                                            std::vector<VkVertexInputAttributeDescription> &
                                            vertexAttributeDescriptions,
                                            VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
                                            uint32_t vertexBinding)
{
  vertexBindingDesc.binding   = 0;
  vertexBindingDesc.stride    = sizeof(VertexAll);
  vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  vertexAttributeDescriptions.resize(8);
  vertexAttributeDescriptions[0].binding  = vertexBinding;
  vertexAttributeDescriptions[0].location = 0;
  vertexAttributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT; // position (vec3)
  vertexAttributeDescriptions[0].offset   = offsetof(VertexAll, position);

  vertexAttributeDescriptions[1].binding  = vertexBinding;
  vertexAttributeDescriptions[1].location = 1;
  vertexAttributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT; // normal (vec3)
  vertexAttributeDescriptions[1].offset   = offsetof(VertexAll, normal);

  vertexAttributeDescriptions[2].binding  = vertexBinding;
  vertexAttributeDescriptions[2].location = 2;
  vertexAttributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT; // uv (vec2)
  vertexAttributeDescriptions[2].offset   = offsetof(VertexAll, uv);

  vertexAttributeDescriptions[3].binding  = vertexBinding;
  vertexAttributeDescriptions[3].location = 3;
  vertexAttributeDescriptions[3].format   = VK_FORMAT_R32G32B32_SFLOAT; // tangent (vec3)
  vertexAttributeDescriptions[3].offset   = offsetof(VertexAll, tangent);

  vertexAttributeDescriptions[4].binding  = vertexBinding;
  vertexAttributeDescriptions[4].location = 4;
  vertexAttributeDescriptions[4].format   = VK_FORMAT_R32G32B32_SFLOAT; // bitangent (vec3)
  vertexAttributeDescriptions[4].offset   = offsetof(VertexAll, bitangent);

  vertexAttributeDescriptions[5].binding  = vertexBinding;
  vertexAttributeDescriptions[5].location = 5;
  vertexAttributeDescriptions[5].format   = VK_FORMAT_R32G32B32A32_SFLOAT; // color (vec4)
  vertexAttributeDescriptions[5].offset   = offsetof(VertexAll, color);

  vertexAttributeDescriptions[6].binding  = vertexBinding;
  vertexAttributeDescriptions[6].location = 6;
  vertexAttributeDescriptions[6].format   = VK_FORMAT_R32G32B32A32_SINT; // boneIndices (ivec4)
  vertexAttributeDescriptions[6].offset   = offsetof(VertexAll, boneIndices);

  vertexAttributeDescriptions[7].binding  = vertexBinding;
  vertexAttributeDescriptions[7].location = 7;
  vertexAttributeDescriptions[7].format   = VK_FORMAT_R32G32B32A32_SFLOAT; // boneWeights (vec4)
  vertexAttributeDescriptions[7].offset   = offsetof(VertexAll, boneWeights);

  vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount   = 1;
  vertexInputInfo.pVertexBindingDescriptions      = &vertexBindingDesc;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions    = vertexAttributeDescriptions.data();
}

void PipelinePool::buildDepthStencilPipeline(VkPipelineDepthStencilStateCreateInfo &depthStencilCi,
                                             VkBool32 depthTestEnable,
                                             VkBool32 stencilTestEnable,
                                             VkBool32 depthWriteEnable,
                                             VkCompareOp depthCompareOp)
{
  depthStencilCi.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilCi.depthTestEnable       = depthTestEnable;
  depthStencilCi.depthWriteEnable      = depthWriteEnable;
  depthStencilCi.depthCompareOp        = depthCompareOp;
  depthStencilCi.depthBoundsTestEnable = VK_FALSE;
  depthStencilCi.stencilTestEnable     = stencilTestEnable;
  //todo : check need the stencil setting
}

void PipelinePool::buildFragmentPipeline(VkPipelineShaderStageCreateInfo &shaderStateCi, VkShaderModule fragModule)
{
  shaderStateCi.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStateCi.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStateCi.module = fragModule;
  shaderStateCi.pName  = "main";
}

void PipelinePool::buildVertexPipeline(VkPipelineShaderStageCreateInfo &shaderStateCi, VkShaderModule vertexModule)
{
  shaderStateCi.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStateCi.stage  = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStateCi.module = vertexModule;
  shaderStateCi.pName  = "main";
}

void PipelinePool::buildAssemblyPipeline(VkPipelineInputAssemblyStateCreateInfo &inputAssembly,
                                         VkPrimitiveTopology topology,
                                         VkBool32 primitiveRestartEnable,
                                         VkPipelineInputAssemblyStateCreateFlags
                                         flags)
{
  inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.flags                  = flags;
  inputAssembly.topology               = topology;
  inputAssembly.primitiveRestartEnable = primitiveRestartEnable;
}

void PipelinePool::buildDynamicPipelineDyscriptor(VkPipelineDynamicStateCreateInfo &dynamicStateCi,
                                                  std::vector<VkDynamicState> &dynamicStates,
                                                  VkPipelineViewportStateCreateInfo &viewportStateCi,
                                                  uint32_t viewCount,
                                                  VkBool32 dynamicDepthTestEnable,
                                                  VkBool32 dynamicStencilTestEnable,
                                                  VkBool32 dynamicStateDepthCompare,
                                                  VkBool32 dynamicStateVetexStride,
                                                  VkPipelineDynamicStateCreateFlags flags
  )
{
  ///todo:
  /// if need -> create View and Scissor with array
  /// update all and create Mulit view mode
  /// temp-> just single view and multi draw call

  viewportStateCi.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCi.viewportCount = viewCount;
  viewportStateCi.scissorCount  = viewCount;

  dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
  dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

  if (dynamicDepthTestEnable) dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE);
  if (dynamicStencilTestEnable) dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE);
  if (dynamicStateDepthCompare) dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_COMPARE_OP);
  if (dynamicStateVetexStride) dynamicStates.push_back(VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE);

  dynamicStateCi.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCi.dynamicStateCount = dynamicStates.size();
  dynamicStateCi.pDynamicStates    = dynamicStates.data();
  dynamicStateCi.flags             = flags;
}

void PipelinePool::buildRasterizationPipeline(VkPipelineRasterizationStateCreateInfo &rasterizaCi,
                                              VkCullModeFlags cullMode,
                                              VkPolygonMode mode,
                                              VkFrontFace front,
                                              VkPipelineRasterizationStateCreateFlags flags)
{
  rasterizaCi.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizaCi.depthClampEnable        = VK_FALSE;
  rasterizaCi.rasterizerDiscardEnable = VK_FALSE;
  rasterizaCi.depthBiasEnable         = VK_FALSE;
  rasterizaCi.lineWidth               = 1.0f;
  rasterizaCi.polygonMode             = mode;
  rasterizaCi.cullMode                = cullMode;
  rasterizaCi.frontFace               = front;
  rasterizaCi.flags                   = flags;
}

void PipelinePool::buildMultiSamplingPipeline(VkPipelineMultisampleStateCreateInfo &multiSamplingCi,
                                              VkSampleCountFlagBits samples,
                                              VkPipelineMultisampleStateCreateFlags flags)
{
  multiSamplingCi.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multiSamplingCi.rasterizationSamples = samples;
  multiSamplingCi.flags                = flags;
}

void PipelinePool::buildColorBlendingAttachment(VkPipelineColorBlendAttachmentState &colorBlendAttachment,
                                                uint32_t flags,
                                                VkBool32 blendEnable)
{ //this can be vector
  colorBlendAttachment.colorWriteMask = flags;
  colorBlendAttachment.blendEnable    = blendEnable;
}

void PipelinePool::buildColorBlendingPipeline(VkPipelineColorBlendStateCreateInfo &colorBlendingCi,
                                              VkPipelineColorBlendAttachmentState attachment,
                                              uint32_t attachmentCount)
{
  colorBlendingCi.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendingCi.attachmentCount = attachmentCount;
  colorBlendingCi.pAttachments    = &attachment;
}

void PipelinePool::buildDynamicRenderingPipeline(VkPipelineRenderingCreateInfo &dynamicRendering,
                                                 VkFormat *colorAttachmentFormats,
                                                 uint32_t colorAttachmentCount,
                                                 uint32_t viewMask,
                                                 VkFormat depthFormat,
                                                 VkFormat stencilAttachment)
{
  dynamicRendering.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  dynamicRendering.pColorAttachmentFormats = colorAttachmentFormats;
  dynamicRendering.colorAttachmentCount    = colorAttachmentCount;
  dynamicRendering.depthAttachmentFormat   = depthFormat;
  dynamicRendering.viewMask                = viewMask;
  dynamicRendering.stencilAttachmentFormat = stencilAttachment;
}

VkPipelineLayout PipelinePool::createPipelineLayout(
    VkDescriptorSetLayout *descriptorLayoutData,
    uint32_t descriptorSetCount
  )
{ //this can be updated by other hash
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = descriptorSetCount;
  pipelineLayoutInfo.pSetLayouts    = descriptorLayoutData;

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
  {
    throw std::runtime_error("devicecreate pipeline layout!");
  }
  return pipelineLayout;
}

VkPipeline PipelinePool::createPipeline(VertexType type,
                                        VkShaderModule vertexModule,
                                        VkShaderModule fragModule,
                                        VkFormat *colorAttachmentFormats,
                                        uint32_t colorAttachmentCount,
                                        uint32_t viewMask,
                                        VkFormat depthFormat,
                                        VkFormat stencilAttachment,
                                        uint32_t layoutCount,
                                        VkDescriptorSetLayout *layoutData,
                                        VkPrimitiveTopology topology,
                                        VkCullModeFlags cullMode,
                                        VkBool32 depthTestEnable,
                                        VkBool32 depthWriteEnable,
                                        VkBool32 stencilTestEnable,
                                        VkCompareOp depthCompareOp,
                                        VkRenderPass renderPass)
{
  PipelineCi ci{};
  //need: dynamic options
  //VkPipelineLayout layout = createPipelineLayout(layoutData, layoutCount);
  buildVertexPipeline(ci.vertShaderStageInfo_, vertexModule);
  buildFragmentPipeline(ci.fragShaderStageInfo_, fragModule);
  buildVertexDescriptor(type,
                        ci.vertexBindingDesc_,
                        ci.vertexAttributeDescriptions_,
                        ci.vertexInputInfo_);

  buildAssemblyPipeline(ci.inputAssembly,
                        topology);

  buildDynamicPipelineDyscriptor(ci.dynamicStateCi,
                                 ci.dynamicStates,
                                 ci.viewportStateCi);

  buildRasterizationPipeline(ci.rasterizeCi, cullMode);
  buildMultiSamplingPipeline(ci.multiSamplingCi);
  buildColorBlendingAttachment(ci.colorBlendAttachment);
  buildColorBlendingPipeline(ci.colorBlendingCi, ci.colorBlendAttachment);

  buildDepthStencilPipeline(ci.depthStencilCi,
                            depthTestEnable,
                            stencilTestEnable,
                            depthWriteEnable,
                            depthCompareOp);

  buildDynamicRenderingPipeline(ci.dynamicRenderingInfo,
                                colorAttachmentFormats,
                                colorAttachmentCount,
                                viewMask,
                                depthFormat,
                                stencilAttachment);

  VkPipelineShaderStageCreateInfo pipelineShaderStageInfo[2] = {ci.vertShaderStageInfo_, ci.fragShaderStageInfo_};
  if (renderPass == VK_NULL_HANDLE) renderPass = renderPass_;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount          = 2;
  pipelineInfo.pStages             = pipelineShaderStageInfo;
  pipelineInfo.pVertexInputState   = &ci.vertexInputInfo_;
  pipelineInfo.pInputAssemblyState = &ci.inputAssembly;
  pipelineInfo.pViewportState      = &ci.viewportStateCi;
  pipelineInfo.pDynamicState       = &ci.dynamicStateCi;
  pipelineInfo.pRasterizationState = &ci.rasterizeCi;
  pipelineInfo.pMultisampleState   = &ci.multiSamplingCi;
  pipelineInfo.pColorBlendState    = &ci.colorBlendingCi;
  pipelineInfo.pDepthStencilState  = &ci.depthStencilCi; //depth append
  pipelineInfo.layout              = basePipelineLayout_;
  pipelineInfo.pNext               = &ci.dynamicRenderingInfo;
  pipelineInfo.renderPass          = renderPass;
  pipelineInfo.subpass             = 0;

  VkPipeline pipeline = VK_NULL_HANDLE;
  VK_ASSERT(vkCreateGraphicsPipelines(device_,
        nullptr,
        1,
        &pipelineInfo,
        nullptr,
        &pipeline)
    );

  PipelineProgram program;
  program.vertShaderModule = ci.vertShaderStageInfo_.module;
  program.fragShaderModule = ci.fragShaderStageInfo_.module;
  program.cullMode         = ci.rasterizeCi.cullMode;
  program.depthTestEnable  = ci.depthStencilCi.depthTestEnable;
  program.depthWriteEnable = ci.depthStencilCi.depthWriteEnable;
  program.topology         = ci.inputAssembly.topology;
  program.depthCompareOp   = ci.depthStencilCi.depthCompareOp;
  pipelineHash_[program]   = pipeline;

  return pipeline;
}

std::vector<uint8_t> PipelinePool::loadPipelineCache(
    const std::string &filename
  )
{
  std::ifstream inFile(filename, std::ios::binary | std::ios::ate);
  if (!inFile.is_open())
  {
    spdlog::info("no pipeline cache data");
    return {};
  }
  std::streamsize size = inFile.tellg();
  inFile.seekg(0, std::ios::beg);
  std::vector<uint8_t> buffer(size);
  if (!inFile.read(reinterpret_cast<char *>(buffer.data()), size))
  {
    spdlog::info("failed to read pipeline cache");
    return {};
  }
  return buffer;
}