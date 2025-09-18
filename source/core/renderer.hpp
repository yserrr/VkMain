#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <common.hpp>
#include <../resource/pipeline.hpp>
#include <../resource/renderpass_pool.hpp>
#include <../resource/descriptor_manager.hpp>
#include  <command_pool_manager.hpp>
#include  <command_pool.hpp>
#include  <frame_pool.hpp>
#include <../resource/shader_module.hpp>
#include  <swapchain_view.hpp>
#include  <swapChain.hpp>
#include  <mesh.hpp>
#include <../resource/sampler_builder.hpp>
#include  <texture.hpp>
#include  <resource_manager.hpp>
#include  <semaphore_pool.hpp>
#include  <fence_pool.hpp>
#include "../../extern/examples/renderer_resource.hpp"

struct RendererCreateInfo{
  GLFWwindow *window;
  VkPhysicalDevice physical_device_h;
  VkDevice device_h;
  VkSurfaceKHR surface_h;
  uint32_t graphics_family;
  uint32_t present_family;
  VkQueue present_q;
  VkQueue graphics_q;
  MemoryAllocator *allocator;
  VkExtent2D extent;
  ResourceManager *asset;
  Swapchain *swapchain;
  SwapchainViewManager *imageManager;
  FramebufferPool *frameManager;
  VkRenderPass renderPass;
};

enum class ViewMode{
  SingleView,
  MultiView,
};

class SceneRenderer{
  friend class Engine;
  friend class EventManager;

public:
  SceneRenderer(RendererCreateInfo info);
  ~SceneRenderer() {}
  void pushConstant(VkCommandBuffer cmdBuffer);
  void createPipeline(std::vector<VkDescriptorSetLayout> *layout_h);
  void setUp(VkCommandBuffer command);
  void draw(VkCommandBuffer command);
  void setCamera(Camera *cameraP)
  {
    camera = cameraP;
  }

  VkPipelineLayout getPipelineLayout()
  {
    return pipeline_layout_h;
  }

private:
  VkPhysicalDevice physical_device_h;
  VkDevice device_h;
  VkQueue graphics_q;
  VkQueue present_q;
  VkSurfaceKHR surface_h;
  VkFormat format;
  VkPipeline pipeline_h;
  VkPipelineLayout pipeline_layout_h;
  VkRenderPass renderpass_h;

  uint32_t present_family;
  uint32_t graphics_family;

  std::vector<BatchContext> batches_;
  ViewMode viewMode = ViewMode::SingleView;
  PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
  VkBool32 depthTest        = VK_TRUE;

  std::unique_ptr<ShaderModule> fragShader;
  std::unique_ptr<ShaderModule> vertShader;
  std::unique_ptr<GraphicsPipeline> pipeline;

  VkViewport viewport{};
  VkRect2D scissor{};
  std::string fragPath = "/home/ljh/CLionProjects/VkMain/source/shader/fragment.frag";
  std::string vertPath = "/home/ljh/CLionProjects/VkMain/source/shader/vertex.vert";
  FramebufferPool *framebufferManager;
  SwapchainViewManager *imageManager;
  Swapchain *swapchain;

  Camera *camera;
  VulkanTexture *texture;
  ResourceManager &resourceManager;
  MemoryAllocator &allocator;
//temp ;
  VkDeviceSize offsets = 0;
};
#endif