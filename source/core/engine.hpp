#ifndef ENGINE_HPP
#define ENGINE_HPP

#include  <common.hpp>
#include  <importer.hpp>
#include <../model/mesh.hpp>
#include  <renderer.hpp>
#include  <ui_renderer.hpp>
#include  <event_manager.hpp>
#include  <resource_manager.hpp>
#include "engine_vk_context.hpp"

class Engine{
public:
  Engine();
  ~Engine();
  void init();
  void run();

private:
  void vkDeviceload();
  void setUp();
  void draw();
  VkCommandBuffer rec(uint32_t imageIndex);
  void summitQueue(VkCommandBuffer command, uint32_t imageIndex);
  std::shared_ptr<UILogSink> UIsink;
  GLFWwindow *window_h;
  VkInstance instance_h;
  VkSurfaceKHR surface_h;
  VkPhysicalDevice physical_device_h;
  VkDevice device_h;
  VkSwapchainKHR swapchain_h;
  VkFormat imageformat_h;
  VkRenderPass renderpass_h;
  VkCommandPool command_pool_h;
  VkQueue present_q;
  VkQueue graphics_q;
  uint32_t graphics_family;
  uint32_t present_family;
  VkPipelineLayout pipeline_layout_h;

  double lastFrameTime;
  uint32_t currentFrame         = 0;
  uint32_t MAX_FRAMES_IN_FLIGHT = 3;
  uint32_t frameCount;
  uint64_t timeout; //similar with float
  uint32_t imageIndex_;
  std::unique_ptr<Swapchain> swapchain;
  std::unique_ptr<SwapchainViewManager> imageManager;
  std::unique_ptr<RenderPassPool> renderPass;
  std::unique_ptr<FramebufferPool> frameBufferManager;
  std::unique_ptr<CommandPool> commandPool;
  std::unique_ptr<CommandPoolManager> commandBufferManager;
  std::unique_ptr<MemoryAllocator> allocator;
  std::unique_ptr<ResourceManager> resourceManager_;
  std::unique_ptr<EventManager> eventManager_;
  std::unique_ptr<UIRenderer> uiRenderer;
  std::unique_ptr<SceneRenderer> sceneRenderer;

  std::unique_ptr<SemaphorePool> imageAvailableSemaphores;
  std::unique_ptr<SemaphorePool> renderFinishedSemaphores;
  std::unique_ptr<FencePool> inFlightFences;

// 기타
  std::vector<VkRenderPassBeginInfo> renderPassInfos;
  std::array<VkClearValue, 2> clearValues;

  VkExtent2D extent  = {2400, 1200};
  const char *title  = "vulkan";
};

#endif //engine_hpp