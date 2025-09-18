//
// Created by ljh on 25. 9. 16..
//

#include "dynamic_renderer.hpp"
#include "renderer_resource.hpp"
void DynamicRenderer::dynamicRendering()
{


  glfwPollEvents();
  if (imageFence[sysRenderContext_.imageIndex] != VK_NULL_HANDLE)
  {
    vkWaitForFences(device_h, 1, &imageFence[sysRenderContext_.imageIndex], VK_TRUE, 10000000000);
  }
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
  } else if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
  {
    throw std::runtime_error("Could not acquire the next swap chain image!");
  }
  gpu::fps_cam camData = fpsCam_.flush();
  std::memcpy(camBufHandle[sysRenderContext_.currentFrame].mapped, &camData, sizeof(gpu::fps_cam));

  eventManager_->getKey();
  eventManager_->wheelUpdate();
  VkCommandBuffer commandBuffer = commandManager_->beginPrimary(sysRenderContext_.currentFrame);

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
                                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

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
                                       VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);

  VkRenderingAttachmentInfo colorAttachment{};
  colorAttachment.sType            = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  colorAttachment.imageView        = swapchain_->getSwapchainImageView(sysRenderContext_.imageIndex);
  colorAttachment.imageLayout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  colorAttachment.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0};

  VkRenderingAttachmentInfo depthAttachment{};
  depthAttachment.sType                   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  depthAttachment.imageView               = frameBuffer_->getDepthView(sysRenderContext_.imageIndex);
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
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sysScenePipeline_h);
  vkCmdBindDescriptorSets(commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          sysPipelineLayout_h,
                          0,
                          1,
                          &sysBindlessDescriptor_h,
                          0,
                          nullptr);

  vkCmdBindDescriptorSets(commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          sysPipelineLayout_h,
                          0,
                          1,
                          &camBufHandle[sysRenderContext_.currentFrame].descriptorSet,
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
                         &batches[0].vertexBuffer,
                         offsets);

  vkCmdBindIndexBuffer(commandBuffer,
                       batches[0].indexBuffer,
                       0,
                       VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer,
                     batches[0].indexCount,
                     1,
                     0,
                     0,
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
                                       VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );

  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass               = sysUiPass_h;
  renderPassBeginInfo.renderArea.offset.x      = 0;
  renderPassBeginInfo.renderArea.offset.y      = 0;
  renderPassBeginInfo.renderArea.extent.width  = swapExtent.width;
  renderPassBeginInfo.renderArea.extent.height = swapExtent.height;
  renderPassBeginInfo.clearValueCount          = 0;
  renderPassBeginInfo.pClearValues             = {0};
  renderPassBeginInfo.framebuffer              = sysRenderContext_.frameBuffers[sysRenderContext_.currentFrame];

  vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sysScenePipeline_h);
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
  VkFence waitFence               = inflightSyncManager_->get(sysRenderContext_.currentFrame);

  imageFence[sysRenderContext_.imageIndex] = waitFence;
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
    ///todo ::recreate depth image
  } else if (result != VK_SUCCESS)
  {
    throw "Could not present the image to the swap chain!";
  }
  sysRenderContext_.currentFrame = (sysRenderContext_.currentFrame + 1) % sysRenderContext_.MAX_FRAMES_IN_FLIGHT;
}