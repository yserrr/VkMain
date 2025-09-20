//
// Created by ljh on 25. 9. 17..
//

#include "renderer.hpp"

SceneRenderer::SceneRenderer(RendererCreateInfo info) : device_h(info.device_h),
                                                        physical_device_h(info.physical_device_h),
                                                        graphics_q(info.graphics_q),
                                                        present_q(info.present_q),
                                                        graphics_family(info.graphics_family),
                                                        present_family(info.present_family),
                                                        surface_h(info.surface_h),
                                                        renderpass_h(info.renderPass),
                                                        framebufferManager(info.frameManager),
                                                        swapchain(info.swapchain),
                                                        imageManager(info.imageManager),
                                                        allocator(*info.allocator),
                                                        resourceManager(*info.asset) {}

void SceneRenderer::pushConstant(VkCommandBuffer command)
{
  vkCmdPushConstants(
      command,
      pipeline_layout_h,
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      0,
      sizeof(MaterialConstant),
      &resourceManager.selectedModel.constant
  );
}

void SceneRenderer::createPipeline(std::vector<VkDescriptorSetLayout> *layout_h)
{
  fragShader = std::make_unique<ShaderModule>(device_h);
  vertShader = std::make_unique<ShaderModule>(device_h);
  fragShader->setShader(fragPath, shaderc_fragment_shader);
  vertShader->setShader(vertPath, shaderc_vertex_shader);
  vkCmdSetPolygonModeEXT = (PFN_vkCmdSetPolygonModeEXT) vkGetDeviceProcAddr(device_h, "vkCmdSetPolygonModeEXT");
  if (!vkCmdSetPolygonModeEXT)
  {
    throw std::runtime_error("vkCmdSetPolygonModeEXT not available!");
  }
  PipelineCreateInfo info;
  info.device               = device_h;
  info.extent               = swapchain->getExtent();
  info.renderPass           = renderpass_h;
  info.vertShaderModule     = vertShader->get();
  info.fragShaderModule     = fragShader->get();
  info.descriptorSetLayouts = layout_h;
  pipeline                  = std::make_unique<GraphicsPipeline>(info);
  pipeline_layout_h         = pipeline->getLayout();
  pipeline_h                = pipeline->get();
  spdlog::info("create pipeline");
}