//
// Created by ljh on 25. 9. 16..
//

#ifndef MYPROJECT_RENDERER_RESOURCE_HPP
#define MYPROJECT_RENDERER_RESOURCE_HPP
#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>
#include <mesh_sub.hpp>

struct RenderContext{
  uint32_t currentFrame         = 0;
  uint32_t MAX_FRAMES_IN_FLIGHT = 3;
  uint32_t imageIndex           = 0;
  VkFormat colorFormat{};
  VkFormat depthFormat{};
  VkClearColorValue defaultClearColor = {{0}};
  std::vector<VkFramebuffer> frameBuffers;
  std::vector<VkRenderPassBeginInfo> renderPassBeginInfos;
  VkExtent2D extent{};
  VkViewport viewport{};
  VkRect2D scissor{};
};

struct BatchContext{
  VkPipeline pipeline     = VK_NULL_HANDLE;
  VkBuffer vertexBuffer   = VK_NULL_HANDLE;
  VkBuffer indexBuffer    = VK_NULL_HANDLE;
  VkBuffer instanceBuffer = VK_NULL_HANDLE;
  uint32_t indexCount      = 0;
  std::vector<Submesh> submeshes;
};


#endif //MYPROJECT_RENDERER_RESOURCE_HPP