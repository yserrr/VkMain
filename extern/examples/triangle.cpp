//
// Created by ljh on 25. 9. 7..
// just module -> upgrade with ps
#include <renderer.hpp>

void SystemRenderer::transferStage()
{
  std::vector<VertexPC> vertices = {
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
  };
  uint32_t vertexBufferSize = static_cast<uint32_t>(vertices.size()) * sizeof(VertexPC);

  std::vector<uint32_t> indices{0, 1, 2};
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
  batch.pipeline     = sysUiPipeline_h;
  batches[0]         = batch;

  StreamingBlock vBlock = streamingBuffer_->acquire(vertexBufferSize, sizeof(VertexPC));
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
  commandManager_->endSingleTime(singleCommand, vkContext_->graphics_q);
}

void SystemRenderer::pipelineSetup()
{
  sysRenderContext_.MAX_FRAMES_IN_FLIGHT = 1;
  std::vector<LayoutBindingInfo> layoutBindingInfos(1);
  layoutBindingInfos[0].bindingIndex = 0;
  layoutBindingInfos[0].usage        = gpu::descriptor_usage::UBO;
  sysDescriptorlayout_h                           = descriptorLayoutPool_->createDescriptorSetLayout(layoutBindingInfos);

  descriptorPool_->setLayout(sysDescriptorlayout_h);
  descriptorPool_->createPool(10, static_cast<uint32_t>(gpu::descriptor_usage::UBO));

  gpu::cam_desc camDesc     = fpsCam_.getDesc();
  VkDeviceSize camAlignment = (camDesc.stride +
                               minUniformBufferOffsetAlignmet_) &
                              ~(minUniformBufferOffsetAlignmet_ - 1);
  VkDeviceSize camSize = camAlignment * camDesc.camMax;

  UboPool_ = std::make_unique<BufferPool>(*allocator_,
                                        BufferType::UNIFORM,
                                        camSize,
                                        AccessPolicy::HostPreferred);

  for (int i = 0; i < sysRenderContext_.MAX_FRAMES_IN_FLIGHT; ++i)
  {
    //upload descriptor set
    VkDescriptorSet set = descriptorPool_->allocate(i);
    UboPool_->createBuffer(camBufHandle[i]);
    descriptorUploader_->UploadUboSet(camBufHandle[i].buffer,
                                 camSize,
                                 set,
                                 0);
    camBufHandle[i].descriptorSet = set;
  }

  vertshader->setShader("/home/ljh/Desktop/MyProject-master/source/back/gpu_vk/render_ps/exp_shader/triangleV.vert",
                        shaderc_shader_kind::shaderc_glsl_vertex_shader);
  fragshader->setShader("/home/ljh/Desktop/MyProject-master/source/back/gpu_vk/render_ps/exp_shader/triangleF.frag",
                        shaderc_shader_kind::shaderc_glsl_fragment_shader);
  VkShaderModule vert = vertshader->get();
  VkShaderModule frag = fragshader->get();

  VkPipelineLayoutCreateInfo pipelineLayoutCI{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  pipelineLayoutCI.setLayoutCount = 1;
  pipelineLayoutCI.pSetLayouts    = &sysDescriptorlayout_h;
  VK_ASSERT(vkCreatePipelineLayout(device_h, &pipelineLayoutCI, nullptr, &sysPipelineLayout_h));

  VkGraphicsPipelineCreateInfo pipelineCI{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
  pipelineCI.layout = sysPipelineLayout_h;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
  };
  inputAssemblyStateCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

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

  VkPipelineColorBlendAttachmentState blendAttachmentState{};
  blendAttachmentState.colorWriteMask = 0xf;
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
  vertexInputBinding.stride    = sizeof(VertexPC);
  vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  // Input attribute bindings describe shader attribute locations and memory layouts
  std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributs{};
  // These match the following shader layout (see triangle.vert):
  //	layout (location = 0) in vec3 inPos;
  //	layout (location = 1) in vec3 inColor;
  // Attribute location 0: Position
  vertexInputAttributs[0].binding  = 0;
  vertexInputAttributs[0].location = 0;
  // Position attribute is three 32 bit signed (SFLOAT) floats (R32 G32 B32)
  vertexInputAttributs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexInputAttributs[0].offset = offsetof(VertexPC, position);
  // Attribute location 1: Color
  vertexInputAttributs[1].binding  = 0;
  vertexInputAttributs[1].location = 1;
  // Color attribute is three 32 bit signed (SFLOAT) floats (R32 G32 B32)
  vertexInputAttributs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexInputAttributs[1].offset = offsetof(VertexPC, color);

  // Vertex input state used for pipeline creation
  VkPipelineVertexInputStateCreateInfo vertexInputStateCI{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  vertexInputStateCI.vertexBindingDescriptionCount   = 1;
  vertexInputStateCI.pVertexBindingDescriptions      = &vertexInputBinding;
  vertexInputStateCI.vertexAttributeDescriptionCount = 2;
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
  VK_ASSERT(vkCreateGraphicsPipelines(device_h, nullptr, 1, &pipelineCI, nullptr, &sysUiPipeline_h ));

}

void SystemRenderer::renderStage()
{
  glfwPollEvents();
  inflightSyncManager_->wait(sysRenderContext_.currentFrame);
  inflightSyncManager_->reset(sysRenderContext_.currentFrame);
  VkSemaphore imageAvailable  = presentSyncManager_->getSingal(sysRenderContext_.currentFrame);
  VkSemaphore renderFinishied = renderSyncManager_->getSingal(sysRenderContext_.currentFrame);
  VkResult result             = vkAcquireNextImageKHR(device_h,
                                          swapchain_->getSwapchain(),
                                          100000,
                                          imageAvailable,
                                          VK_NULL_HANDLE,
                                          &sysRenderContext_.imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    swapchain_->recreateSwapchain();
    return;
  }
  else if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
  {
    throw std::runtime_error("Could not acquire the next swap chain image!");
  }
  gpu::fps_cam camData = fpsCam_.flush();
  std::memcpy(camBufHandle[sysRenderContext_.currentFrame].mapped, &camData, sizeof(gpu::fps_cam));
  descriptorUploader_->UploadUboSet(camBufHandle[sysRenderContext_.currentFrame].buffer,
                               camBufHandle[sysRenderContext_.currentFrame].allocation.size,
                               camBufHandle[sysRenderContext_.currentFrame].descriptorSet,
                               0,
                               1
                              );

  VkCommandBuffer commandBuffer                 = commandManager_->beginPrimary(sysRenderContext_.currentFrame);
  VkImageSubresourceRange colorSubresourceRange = {};
  colorSubresourceRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
  colorSubresourceRange.baseMipLevel            = 0;
  colorSubresourceRange.levelCount              = 1;
  colorSubresourceRange.layerCount              = 1;

  streamingBuffer_->recordImageBarrier(commandBuffer,
                                       swapchain_->getImage(sysRenderContext_.imageIndex),
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
                                       frameBuffer_->getDepthImage(sysRenderContext_.imageIndex),
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

  // New structures are used to define the attachments used in dynamic rendering
  // Color attachment
  VkRenderingAttachmentInfo colorAttachment{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
  colorAttachment.imageView        = swapchain_->getSwapchainImageView(sysRenderContext_.imageIndex);
  colorAttachment.imageLayout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  colorAttachment.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.clearValue.color = {0.0f, 0.0f, 0.2f, 0.0f};
  // Depth/stencil attachment

  VkRenderingAttachmentInfo depthStencilAttachment{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
  depthStencilAttachment.imageView               = frameBuffer_->getDepthView(sysRenderContext_.imageIndex);
  depthStencilAttachment.imageLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  depthStencilAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthStencilAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthStencilAttachment.clearValue.depthStencil = {1.0f, 0};

  VkRenderingInfo renderingInfo{VK_STRUCTURE_TYPE_RENDERING_INFO_KHR};
  renderingInfo.layerCount           = 1;
  renderingInfo.colorAttachmentCount = 1;
  renderingInfo.pColorAttachments    = &colorAttachment;
  renderingInfo.pDepthAttachment     = &depthStencilAttachment;
  renderingInfo.pStencilAttachment   = nullptr;
  renderingInfo.renderArea           = {
    0,
    0,
    swapchain_->getExtent().width,
    swapchain_->getExtent().height
  };
  VkExtent2D swapExtent = swapchain_->getExtent();
  vkCmdBeginRendering(commandBuffer, &renderingInfo);

  VkViewport viewport{
    0.0f,
    0.0f,
    (float)swapExtent.width,
    (float)swapExtent.height,
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
  vkCmdBindDescriptorSets(commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          sysPipelineLayout_h,
                          0,
                          1,
                          &camBufHandle[sysRenderContext_.currentFrame].descriptorSet,
                          0,
                          nullptr);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sysUiPipeline_h);
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
                                       swapchain_->getImage(sysRenderContext_.imageIndex),
                                       renderResourceRange,
                                       VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                       0,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
                                      );
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
  VkFence waitFence               = inflightSyncManager_->get(sysRenderContext_.currentFrame);
  VK_ASSERT(vkQueueSubmit(vkContext_->graphics_q, 1, &submitInfo, waitFence));

  VkSwapchainKHR swapchain_h = (swapchain_->getSwapchain());

  VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = &renderFinishied;
  presentInfo.swapchainCount     = 1;
  presentInfo.pSwapchains        = &swapchain_h;
  presentInfo.pImageIndices      = &sysRenderContext_.imageIndex;
  result                         = vkQueuePresentKHR(vkContext_->present_q, &presentInfo);
  if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
  {
    swapchain_->recreateSwapchain();
  }
  else if (result != VK_SUCCESS)
  {
    throw "Could not present the image to the swap chain!";
  }
  sysRenderContext_.currentFrame = (sysRenderContext_.currentFrame + 1) % sysRenderContext_.MAX_FRAMES_IN_FLIGHT;
}


int main()
{
  SystemRenderer renderer;
  renderer.render();
}
