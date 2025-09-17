#include<renderer.hpp>

void SceneRenderer::setUp(VkCommandBuffer command)
{
  vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h); // 파이프라인 바인딩
  vkCmdSetPolygonModeEXT(command, polygonMode);
  vkCmdSetDepthTestEnable(command, depthTest);
  camera->camUpdate();
  viewport.x        = 0.0f;
  viewport.y        = 0.0f;
  viewport.width    = (float) swapchain->getExtent().width;
  viewport.height   = (float) swapchain->getExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(command, 0, 1, &viewport);
  scissor.offset = {0, 0};
  scissor.extent = swapchain->getExtent();
  vkCmdSetScissor(command, 0, 1, &scissor);
}

void SceneRenderer::draw(VkCommandBuffer command)
{
  vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h); // 파이프라인 바인딩
  vkCmdSetPolygonModeEXT(command, polygonMode);
  vkCmdSetDepthTestEnable(command, depthTest);
  if (viewMode == ViewMode::MultiView)
  {
    // swapchain 크기 가져오기
    auto extent      = swapchain->getExtent();
    float halfWidth  = extent.width / 2.0f;
    float halfHeight = extent.height / 2.0f;
    for (uint32_t i = 0; i < 4; i++)
    {
      // viewport 설정
      viewport.width    = halfWidth;
      viewport.height   = halfHeight;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      viewport.x = (i % 2) * halfWidth;
      viewport.y = (i / 2) * halfHeight;
      vkCmdSetViewport(command, 0, 1, &viewport);

      scissor.offset = {static_cast<int32_t>(viewport.x), static_cast<int32_t>(viewport.y)};
      scissor.extent = {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)};
      vkCmdSetScissor(command, 0, 1, &scissor);
      for (auto &mesh: resourceManager.meshes_)
      {
        mesh.second->bind(command);
        mesh.second->draw(command);
      }
    }
  } else
  {
    camera->camUpdate();
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = (float) swapchain->getExtent().width;
    viewport.height   = (float) swapchain->getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command, 0, 1, &viewport);

    scissor.offset = {0, 0};
    scissor.extent = swapchain->getExtent();
    vkCmdSetScissor(command, 0, 1, &scissor);

    for (auto &mesh: resourceManager.meshes_)
    {
      mesh.second->bind(command);
      mesh.second->draw(command);
    }
  }
}

void SceneRenderer::pushConstant(VkCommandBuffer command, Material material)
{
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset     = 0;
  // pushConstantRange.size       = sizeof(MaterialType);
  //setting materal for rendereing material constant setting

  // MaterialType materialData = {};
  // materialData.albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
  // materialData.params = glm::vec4(0.5f, 0.3f, 1.0f, 0.0f);  // metallic, roughness, ao, padding
  // materialData.flags = (1 << 0); // 예: useAlbedoMap 플래그

  // vkCmdPushConstants(
  // command,
  // layout,
  // VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
  // 0,
  // sizeof(MaterialType),
  // &materialData
  //);
}