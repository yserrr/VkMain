//
// Created by ljh on 25. 9. 7..
// just module -> upgrade with ps
#include <renderer.hpp>
#include <ktxvulkan.h>

void SystemRenderer::pipelineSetup()
{
  renderContext.MAX_FRAMES_IN_FLIGHT = 1;
  std::vector<LayoutBindingInfo> layoutBindingInfos(2);
  layoutBindingInfos[0].bindingIndex = 0;
  layoutBindingInfos[0].usage        = gpu::descriptor_usage::UBO;

  layoutBindingInfos[1].bindingIndex = 1;
  layoutBindingInfos[1].usage        = gpu::descriptor_usage::TEXTURE_BINDLESS;

  layout_h = descriptorLayoutPool_->createDescriptorSetLayout(layoutBindingInfos);

  uint32_t usage = gpu::descriptor_usage::UBO | gpu::descriptor_usage::TEXTURE_BINDLESS;

  descriptorPool_->setLayout(layout_h);
  descriptorPool_->createPool(10, usage);

  gpu::cam_desc camDesc     = fpsCam_.getDesc();
  VkDeviceSize camAlignment = (camDesc.stride +
                               minUniformBufferOffsetAlignmet_) &
                              ~(minUniformBufferOffsetAlignmet_ - 1);
  VkDeviceSize camSize = camAlignment * camDesc.camMax;

  UboPool_ = std::make_unique<BufferPool>(*allocator_,
                                        BufferType::UNIFORM,
                                        camSize,
                                        AccessPolicy::HostPreferred);

  for (int i = 0; i < renderContext.MAX_FRAMES_IN_FLIGHT; i++)
  {
    VkDescriptorSet set = descriptorPool_->allocate(0);
    UboPool_->createBuffer(camBufHandle[i]);
    camBufHandle[i].descriptorSet = set;
    camBufHandle[i].bindingIndex  = 0;
    descriptorUploader_->UploadUboSet(
      camBufHandle[i].buffer,
      camBufHandle[i].allocation.size,
      camBufHandle[i].descriptorSet,
      camBufHandle[i].bindingIndex,
      1);
  }

  vertshader->setShader("/home/ljh/Desktop/MyProject-master/source/back/gpu_vk/render_ps/exp_shader/textureV.vert",
                        shaderc_shader_kind::shaderc_glsl_vertex_shader);
  fragshader->setShader("/home/ljh/Desktop/MyProject-master/source/back/gpu_vk/render_ps/exp_shader/textureF.frag",
                        shaderc_shader_kind::shaderc_glsl_fragment_shader);

  VkShaderModule vert = vertshader->get();
  VkShaderModule frag = fragshader->get();

  VkPipelineLayoutCreateInfo pipelineLayoutCI{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  pipelineLayoutCI.setLayoutCount = 1;
  pipelineLayoutCI.pSetLayouts    = &layout_h;
  VK_ASSERT(vkCreatePipelineLayout(device_h, &pipelineLayoutCI, nullptr, &pipelineLayout_h));

  VkGraphicsPipelineCreateInfo pipelineCI{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
  pipelineCI.layout = pipelineLayout_h;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{
  VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
  };
  inputAssemblyStateCI.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyStateCI.primitiveRestartEnable = VK_FALSE;
  inputAssemblyStateCI.flags                  = 0;

  VkPipelineRasterizationStateCreateInfo rasterizationStateCI{
  VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
  };
  rasterizationStateCI.polygonMode             = VK_POLYGON_MODE_FILL;
  rasterizationStateCI.cullMode                = VK_CULL_MODE_NONE;
  rasterizationStateCI.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizationStateCI.depthClampEnable        = VK_FALSE;
  rasterizationStateCI.rasterizerDiscardEnable = VK_FALSE;
  rasterizationStateCI.depthBiasEnable         = VK_FALSE;
  rasterizationStateCI.lineWidth               = 1.0f;
  rasterizationStateCI.flags                   = 0;

  VkPipelineColorBlendAttachmentState blendAttachmentState{};
  blendAttachmentState.colorWriteMask = COLOR_FLAG_ALL;
  blendAttachmentState.blendEnable    = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlendStateCI{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
  colorBlendStateCI.attachmentCount = 1;
  colorBlendStateCI.pAttachments    = &blendAttachmentState;

  VkPipelineViewportStateCreateInfo viewportStateCI{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
  viewportStateCI.viewportCount = 1;
  viewportStateCI.scissorCount  = 1;

  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicStateCI{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
  dynamicStateCI.pDynamicStates    = dynamicStateEnables.data();
  dynamicStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

  VkPipelineDepthStencilStateCreateInfo depthStencilStateCI{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
  depthStencilStateCI.depthTestEnable       = VK_TRUE;
  depthStencilStateCI.depthWriteEnable      = VK_TRUE;
  depthStencilStateCI.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
  depthStencilStateCI.depthBoundsTestEnable = VK_FALSE;
  depthStencilStateCI.back.failOp           = VK_STENCIL_OP_KEEP;
  depthStencilStateCI.back.passOp           = VK_STENCIL_OP_KEEP;
  depthStencilStateCI.back.compareOp        = VK_COMPARE_OP_ALWAYS;
  depthStencilStateCI.stencilTestEnable     = VK_FALSE;
  depthStencilStateCI.front                 = depthStencilStateCI.back;

  VkPipelineMultisampleStateCreateInfo multisampleStateCI{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
  multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  // Vertex input binding
  // This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
  VkVertexInputBindingDescription vertexInputBinding{};
  vertexInputBinding.binding   = 0;
  vertexInputBinding.stride    = sizeof(VertexPUVN);
  vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  // Input attribute bindings describe shader attribute locations and memory layouts
  std::array<VkVertexInputAttributeDescription, 3> vertexInputAttributs{};
  // These match the following shader layout (see triangle.vert):
  //	layout (location = 0) in vec3 inPos;
  //	layout (location = 1) in vec3 inColor;
  // Attribute location 0: Position
  vertexInputAttributs[0].binding  = 0;
  vertexInputAttributs[0].location = 0;
  vertexInputAttributs[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
  vertexInputAttributs[0].offset   = offsetof(VertexPUVN, position);

  vertexInputAttributs[1].binding  = 0;
  vertexInputAttributs[1].location = 1;
  vertexInputAttributs[1].format   = VK_FORMAT_R32G32_SFLOAT;
  vertexInputAttributs[1].offset   = offsetof(VertexPUVN, uv);

  vertexInputAttributs[2].binding  = 0;
  vertexInputAttributs[2].location = 1;
  vertexInputAttributs[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
  vertexInputAttributs[2].offset   = offsetof(VertexPUVN, normal);

  // Vertex input state used for pipeline creation
  VkPipelineVertexInputStateCreateInfo vertexInputStateCI{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  vertexInputStateCI.vertexBindingDescriptionCount   = 1;
  vertexInputStateCI.pVertexBindingDescriptions      = &vertexInputBinding;
  vertexInputStateCI.vertexAttributeDescriptionCount = 3;
  vertexInputStateCI.pVertexAttributeDescriptions    = vertexInputAttributs.data();

  // Shaders
  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

  // Vertex shader
  shaderStages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStages[0].module = vertshader->get();
  shaderStages[0].pName  = "main";
  assert(shaderStages[0].module != VK_NULL_HANDLE);

  // Fragment shader
  shaderStages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStages[1].module = fragshader->get();
  shaderStages[1].pName  = "main";
  assert(shaderStages[1].module != VK_NULL_HANDLE);

  // Set pipeline shader stage info
  pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
  pipelineCI.pStages    = shaderStages.data();

  // Attachment information for dynamic rendering
  VkFormat format = swapchain_->getFormat();

  VkPipelineRenderingCreateInfoKHR pipelineRenderingCI{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR};
  pipelineRenderingCI.colorAttachmentCount    = 1;
  pipelineRenderingCI.pColorAttachmentFormats = &format;
  pipelineRenderingCI.depthAttachmentFormat   = VK_FORMAT_D32_SFLOAT;
  pipelineRenderingCI.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

  // Assign the pipeline states to the pipeline creation info structure
  pipelineCI.pVertexInputState   = &vertexInputStateCI;
  pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
  pipelineCI.pRasterizationState = &rasterizationStateCI;
  pipelineCI.pColorBlendState    = &colorBlendStateCI;
  pipelineCI.pMultisampleState   = &multisampleStateCI;
  pipelineCI.pViewportState      = &viewportStateCI;
  pipelineCI.pDepthStencilState  = &depthStencilStateCI;
  pipelineCI.pDynamicState       = &dynamicStateCI;
  pipelineCI.pNext               = &pipelineRenderingCI;
  VK_ASSERT(vkCreateGraphicsPipelines(device_h, nullptr, 1, &pipelineCI, nullptr, &pipeline_h ));
  pipelineCI.renderPass = uiPass;
  pipelineCI.subpass    = 0;
  VK_ASSERT(vkCreateGraphicsPipelines(device_h, nullptr, 1, &pipelineCI, nullptr, &ImguiRenderPipelineHandle ));
}

void SystemRenderer::transferStage()
{
  std::vector<VertexPUVN> vertices = {
  {
  {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
  {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
  {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
  {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
  }
  };

  uint32_t vertexBufferSize = static_cast<uint32_t>(vertices.size()) * sizeof(VertexPUVN);
  std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
  uint32_t indexCount = static_cast<uint32_t>(indices.size());
  uint32_t indexBufferSize = indexCount * sizeof(uint32_t);
  vBuffer_ = std::make_unique<BufferPool>(*allocator_, BufferType::VERTEX, vertexBufferSize, AccessPolicy::DeviceLocal);
  iBuffer_ = std::make_unique<BufferPool>(*allocator_, BufferType::INDEX, indexBufferSize, AccessPolicy::DeviceLocal);
  vBuffer_->create();
  iBuffer_->create();

  Submesh submesh{};
  submesh.indexCount   = indexCount;
  submesh.startIndex   = 0;
  submesh.vertexOffset = 0;

  BatchContext batch{};
  batch.indexBuffer  = iBuffer_->getBuffer();
  batch.vertexBuffer = vBuffer_->getBuffer();
  batch.submeshes    = {submesh};
  batch.pipeline     = pipeline_h;
  batches[0]         = batch;

  StreamingBlock vBlock = streamingBuffer_->acquire(vertexBufferSize, sizeof(VertexPUVN));
  StreamingBlock iBlock = streamingBuffer_->acquire(indexBufferSize, sizeof(uint32_t));
  streamingBuffer_->map(vertices.data(), vBlock.offset, vBlock.size);
  streamingBuffer_->map(indices.data(), iBlock.offset, iBlock.size);

  VkSingleCommandBuffer singleCommand = commandManager_->beginSingleTime();
  streamingBuffer_->recordCopyToBuffer(singleCommand, vBlock, vBuffer_->getBuffer(), 0);
  streamingBuffer_->recordCopyToBuffer(singleCommand, iBlock, iBuffer_->getBuffer(), 0);
  streamingBuffer_->recordBufferBarrier(singleCommand,
                                        VK_ACCESS_TRANSFER_WRITE_BIT,
                                        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                                        vBuffer_->getBuffer(),
                                        0,
                                        vertexBufferSize);

  streamingBuffer_->recordBufferBarrier(singleCommand,
                                        VK_ACCESS_TRANSFER_WRITE_BIT,
                                        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                                        iBuffer_->getBuffer(),
                                        0,
                                        indexBufferSize);
  ///texture setting

  std::string texturePath =
  "/home/ljh/Desktop/MyProject-master/extern/externModel/assets/textures/metalplate01_rgba.ktx";
  VkFormat format        = VK_FORMAT_R8G8B8A8_UNORM;
  ktxTexture *ktxTexture = nullptr;
  ktxResult result;
  result = ktxTexture_CreateFromNamedFile(texturePath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
  assert(result == KTX_SUCCESS);
  VulkanTexture texture{};
  texture.width                = ktxTexture->baseWidth;
  texture.height               = ktxTexture->baseHeight;
  texture.mipLevels            = ktxTexture->numLevels;
  texture.descriptorArrayIndex = 0;
  texture.bindigIndex          = 1;

  ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
  ktx_size_t ktxTextureSize   = ktxTexture_GetSize(ktxTexture);

  StreamingBlock imgBlock = streamingBuffer_->acquire(ktx_size_t(ktxTextureSize), MinAlignType::IMAGE);
  streamingBuffer_->map(ktxTextureData, imgBlock.offset, ktx_size_t(ktxTextureSize));

  std::vector<VkBufferImageCopy> bufferCopyRegions;
  for (uint32_t i = 0; i < texture.mipLevels; i++)
  {
    // Calculate offset into staging buffer for the current mip level
    ktx_size_t offset;
    KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
    assert(ret == KTX_SUCCESS);
    // Setup a buffer image copy structure for the current mip level
    VkBufferImageCopy bufferCopyRegion               = {};
    bufferCopyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.mipLevel       = i;
    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
    bufferCopyRegion.imageSubresource.layerCount     = 1;
    bufferCopyRegion.imageExtent.width               = ktxTexture->baseWidth >> i;
    bufferCopyRegion.imageExtent.height              = ktxTexture->baseHeight >> i;
    bufferCopyRegion.imageExtent.depth               = 1;
    bufferCopyRegion.bufferOffset                    = offset;
    bufferCopyRegions.push_back(bufferCopyRegion);
  }

  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format        = format;
  imageCreateInfo.mipLevels     = texture.mipLevels;
  imageCreateInfo.arrayLayers   = 1;
  imageCreateInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageCreateInfo.extent        = {texture.width, texture.height, 1};
  imageCreateInfo.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  Allocation imgAlloc;
  VK_ASSERT(allocator_->createImage(&imageCreateInfo,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture.img, &imgAlloc));
  VkImageSubresourceRange subresourceRange = {};
  subresourceRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel            = 0;
  subresourceRange.levelCount              = texture.mipLevels;
  subresourceRange.layerCount              = 1;
  streamingBuffer_->recordImageBarrier(singleCommand,
                                       texture.img,
                                       subresourceRange,
                                       0,
                                       VK_ACCESS_TRANSFER_WRITE_BIT,
                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       VK_PIPELINE_STAGE_HOST_BIT,
                                       VK_PIPELINE_STAGE_TRANSFER_BIT
    );

  streamingBuffer_->recordCopyToImage(singleCommand,
                                      imgBlock,
                                      bufferCopyRegions.data(),
                                      bufferCopyRegions.size(),
                                      texture.img,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );
  streamingBuffer_->recordImageBarrier(singleCommand,
                                       texture.img,
                                       subresourceRange,
                                       VK_ACCESS_TRANSFER_WRITE_BIT,
                                       VK_ACCESS_SHADER_READ_BIT,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
    );

  VkSamplerCreateInfo sampler{};
  sampler.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler.maxAnisotropy    = 1.0f;
  sampler.magFilter        = VK_FILTER_LINEAR;
  sampler.minFilter        = VK_FILTER_LINEAR;
  sampler.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler.addressModeU     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler.addressModeV     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler.addressModeW     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler.mipLodBias       = 0.0f;
  sampler.compareOp        = VK_COMPARE_OP_NEVER;
  sampler.minLod           = 0.0f;
  sampler.maxLod           = texture.mipLevels;
  sampler.maxAnisotropy    = physicalDeviceProperties_.limits.maxSamplerAnisotropy;
  sampler.anisotropyEnable = VK_TRUE;
  sampler.borderColor      = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  VK_ASSERT(vkCreateSampler(device_h, &sampler, nullptr, &texture.sampler));

  VkImageViewCreateInfo view{};
  view.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
  view.format                          = format;
  view.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  view.subresourceRange.baseMipLevel   = 0;
  view.subresourceRange.baseArrayLayer = 0;
  view.subresourceRange.layerCount     = 1;
  view.subresourceRange.levelCount     = texture.mipLevels;
  view.image                           = texture.img;

  VK_ASSERT(vkCreateImageView(device_h, &view, nullptr, &texture.view));
  bindlessDescriptor_ = descriptorPool_->allocateBindlessSet(100);

  std::vector<VkDescriptorImageInfo> textureDescriptors(textures_.size());
  for (size_t i = 0; i < textures_.size(); i++)
  {
    textureDescriptors[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    textureDescriptors[i].sampler     = textures_[i]->sampler;;
    textureDescriptors[i].imageView   = textures_[i]->view;
  }
  VkWriteDescriptorSet textureArrayDescriptorWrite = {};
  textureArrayDescriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  textureArrayDescriptorWrite.dstBinding           = 1;
  textureArrayDescriptorWrite.dstArrayElement      = 0;
  textureArrayDescriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  textureArrayDescriptorWrite.descriptorCount      = static_cast<uint32_t>(textures_.size());
  textureArrayDescriptorWrite.pBufferInfo          = 0;
  textureArrayDescriptorWrite.pImageInfo           = textureDescriptors.data();
  textureArrayDescriptorWrite.dstSet               = bindlessDescriptor_;
  vkUpdateDescriptorSets(device_h,
                         static_cast<uint32_t>(textureDescriptors.size()),
                         &textureArrayDescriptorWrite,
                         textureDescriptors.size(),
                         nullptr);

  texture.descriptor = bindlessDescriptor_;
  textures_.push_back(std::make_unique<VulkanTexture>(std::move(texture)));
  commandManager_->endSingleTime(singleCommand, vkContext_->graphics_q);

  vkDeviceWaitIdle(device_h);
  descriptorUploader_->uploadBindlessTextureSet(&texture);
  descriptorUploader_->update();
}

void SystemRenderer::renderStage()
{
  glfwPollEvents();
  if (fence[renderContext.imageIndex] != VK_NULL_HANDLE)
  {
    vkWaitForFences(device_h, 1, &fence[renderContext.imageIndex], VK_TRUE, 10000000000);
  }
  inflightSyncManager_->wait(renderContext.currentFrame);
  inflightSyncManager_->reset(renderContext.currentFrame);
  VkSemaphore imageAvailable  = presentSyncManager_->getSingal(renderContext.currentFrame);
  VkSemaphore renderFinishied = renderSyncManager_->getSingal(renderContext.currentFrame);
  VkResult result             = vkAcquireNextImageKHR(device_h,
                                                      swapchain_->getSwapchain(),
                                                      100000,
                                                      imageAvailable,
                                                      VK_NULL_HANDLE,
                                                      &renderContext.imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    swapchain_->recreateSwapchain();
    return;
  } else if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
  {
    throw std::runtime_error("Could not acquire the next swap chain image!");
  }
  gpu::fps_cam camData = fpsCam_.flush();
  std::memcpy(camBufHandle[renderContext.currentFrame].mapped, &camData, sizeof(gpu::fps_cam));

  eventManager_->getKey();
  eventManager_->wheelUpdate();
  VkCommandBuffer commandBuffer = commandManager_->beginPrimary(renderContext.currentFrame);

  VkImageSubresourceRange colorSubresourceRange = {};
  colorSubresourceRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
  colorSubresourceRange.baseMipLevel            = 0;
  colorSubresourceRange.levelCount              = 1;
  colorSubresourceRange.layerCount              = 1;

  streamingBuffer_->recordImageBarrier(commandBuffer,
                                       swapchain_->getImage(renderContext.imageIndex),
                                       colorSubresourceRange,
                                       0,
                                       VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );

  VkImageSubresourceRange depthSubresourceRange = {};
  depthSubresourceRange.aspectMask              = VK_IMAGE_ASPECT_DEPTH_BIT;
  depthSubresourceRange.baseMipLevel            = 0;
  depthSubresourceRange.levelCount              = 1;
  depthSubresourceRange.layerCount              = 1;

  streamingBuffer_->recordImageBarrier(commandBuffer,
                                       frameBuffer_->getDepthImage(renderContext.imageIndex),
                                       depthSubresourceRange,
                                       0,
                                       VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                                       VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                                       VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                       VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                                       VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
    );

  VkRenderingAttachmentInfo colorAttachment{};
  colorAttachment.sType            = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  colorAttachment.imageView        = swapchain_->getSwapchainImageView(renderContext.imageIndex);
  colorAttachment.imageLayout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  colorAttachment.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0};

  VkRenderingAttachmentInfo depthAttachment{};
  depthAttachment.sType                   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  depthAttachment.imageView               = frameBuffer_->getDepthView(renderContext.imageIndex);
  depthAttachment.imageLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  depthAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.clearValue.depthStencil = {1.0f, 0};

  VkRenderingInfo renderingInfo{VK_STRUCTURE_TYPE_RENDERING_INFO_KHR};
  renderingInfo.layerCount           = 1;
  renderingInfo.colorAttachmentCount = 1;
  renderingInfo.pColorAttachments    = &colorAttachment;
  renderingInfo.pDepthAttachment     = &depthAttachment;
  renderingInfo.pStencilAttachment   = nullptr;
  renderingInfo.renderArea           = {
  0,
  0,
  swapchain_->getExtent().width,
  swapchain_->getExtent().height
  };
  VkExtent2D swapExtent = swapchain_->getExtent();
  vkCmdBeginRendering(commandBuffer, &renderingInfo);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h);

  vkCmdBindDescriptorSets(commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout_h,
                          0,
                          1,
                          &bindlessDescriptor_,
                          0,
                          nullptr);

  vkCmdBindDescriptorSets(commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout_h,
                          0,
                          1,
                          &camBufHandle[renderContext.currentFrame].descriptorSet,
                          0,
                          nullptr);
  VkViewport viewport{
  0.0f,
  0.0f,
  (float) swapExtent.width,
  (float) swapExtent.height,
  0.0f,
  1.0f
  };
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  VkRect2D scissor{
  0,
  0,
  swapExtent.width,
  swapExtent.height
  };
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  VkDeviceSize offsets[1]{0};
  vkCmdBindVertexBuffers(commandBuffer,
                         0,
                         1,
                         vBuffer_->getBufferPtr(),
                         offsets);

  vkCmdBindIndexBuffer(commandBuffer,
                       iBuffer_->getBuffer(),
                       0,
                       VK_INDEX_TYPE_UINT32);

  vkCmdDrawIndexed(commandBuffer,
                   batches[0].submeshes[0].indexCount,
                   1,
                   batches[0].submeshes[0].startIndex,
                   batches[0].submeshes[0].vertexOffset,
                   0);

  vkCmdEndRendering(commandBuffer);

  VkImageSubresourceRange renderResourceRange = {
  VK_IMAGE_ASPECT_COLOR_BIT,
  0,
  1,
  0,
  1
  };

  streamingBuffer_->recordImageBarrier(commandBuffer,
                                       swapchain_->getImage(renderContext.imageIndex),
                                       renderResourceRange,
                                       VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                       VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );

  VkImageView attachments[] = {
  swapchain_->getSwapchainImageView(renderContext.imageIndex),
  };

  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass      = uiPass; // 이전에 만든 render pass
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments    = attachments;
  framebufferInfo.width           = swapExtent.width; // swapchain 또는 원하는 크기
  framebufferInfo.height          = swapExtent.height;
  framebufferInfo.layers          = 1;

  VkFramebuffer framebuffer;
  if (vkCreateFramebuffer(device_h, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create framebuffer!");
  }

  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass               = uiPass;
  renderPassBeginInfo.renderArea.offset.x      = 0;
  renderPassBeginInfo.renderArea.offset.y      = 0;
  renderPassBeginInfo.renderArea.extent.width  = swapExtent.width;
  renderPassBeginInfo.renderArea.extent.height = swapExtent.height;
  renderPassBeginInfo.clearValueCount          = 0;
  renderPassBeginInfo.pClearValues             = {0};
  renderPassBeginInfo.framebuffer              = framebuffer;
  vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ImguiRenderPipelineHandle);
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  uiRenderer_->draw(commandBuffer, *controler_);
  vkCmdEndRenderPass(commandBuffer);
  commandManager_->end(commandBuffer);

  VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};

  submitInfo.pWaitDstStageMask    = &waitStageMask;
  submitInfo.pCommandBuffers      = &commandBuffer;
  submitInfo.commandBufferCount   = 1;
  submitInfo.pWaitSemaphores      = &imageAvailable;
  submitInfo.waitSemaphoreCount   = 1;
  submitInfo.pSignalSemaphores    = &renderFinishied;
  submitInfo.signalSemaphoreCount = 1;

  VkFence waitFence               = inflightSyncManager_->get(renderContext.currentFrame);
  fence[renderContext.imageIndex] = waitFence;
  VK_ASSERT(vkQueueSubmit(vkContext_->graphics_q, 1, &submitInfo, waitFence));
  VkSwapchainKHR swapchain_h = (swapchain_->getSwapchain());

  VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = &renderFinishied;
  presentInfo.swapchainCount     = 1;
  presentInfo.pSwapchains        = &swapchain_h;
  presentInfo.pImageIndices      = &renderContext.imageIndex;
  result                         = vkQueuePresentKHR(vkContext_->present_q, &presentInfo);
  if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
  {
    swapchain_->recreateSwapchain();
    ///todo ::recreate depth image
  } else if (result != VK_SUCCESS)
  {
    throw "Could not present the image to the swap chain!";
  }
  renderContext.currentFrame = (renderContext.currentFrame + 1) % renderContext.MAX_FRAMES_IN_FLIGHT;
}

int main()
{
  SystemRenderer renderer;
  renderer.render();
}