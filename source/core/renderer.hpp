#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <common.hpp>
#include <pipeline.hpp>
#include <renderpass.hpp>
#include <descriptorPoolManager.hpp>
#include  <command_pool_manager.hpp>
#include  <command_pool.hpp>
#include  <frame_pool.hpp>
#include <../resource/shader_module.hpp>
#include  <imageView.hpp>
#include  <swapChain.hpp>
#include  <mesh.hpp>
#include <../resource/sampler_builder.hpp>
#include  <texture.hpp>
#include  <resource_manager.hpp>
#include  <semaphore_pool.hpp>
#include  <fence_pool.hpp>

struct RendererCreateInfo{
  GLFWwindow *        window;
  VkPhysicalDevice    physical_device_h;
  VkDevice            device_h;
  VkSurfaceKHR        surface_h;
  uint32_t            graphics_family;
  uint32_t            present_family;
  VkQueue             present_q;
  VkQueue             graphics_q;
  MemoryAllocator *   allocator;
  VkExtent2D          extent;
  ResourceManager *   asset;
  Swapchain *         swapchain;
  ImageViewManager *  imageManager;
  FramebufferPool *frameManager;
  VkRenderPass        renderPass;
};

enum class ViewMode{
  SingleView,
  MultiView,
};


class SceneRenderer{
  friend class EventManager;
public:
//init for rendering
  SceneRenderer(RendererCreateInfo info)
    : device_h(info.device_h),
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

  ~SceneRenderer() {}

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
//render resource::
//handler : device and Queue hander
  VkPhysicalDevice physical_device_h;
  VkDevice         device_h;
  VkQueue          graphics_q;
  VkQueue          present_q;
  VkSurfaceKHR     surface_h;
  VkFormat         format;
  VkPipeline       pipeline_h;
  VkPipelineLayout pipeline_layout_h;
  VkRenderPass     renderpass_h;

  uint32_t present_family;
  uint32_t graphics_family;
  ViewMode viewMode = ViewMode::SingleView;
  PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
  VkBool32 depthTest = VK_TRUE;


//rendering commander
//render pipeline
  std::unique_ptr<ShaderModule>     fragShader;
  std::unique_ptr<ShaderModule>     vertShader;
  std::unique_ptr<GraphicsPipeline> pipeline;

  VkViewport viewport{};
  VkRect2D scissor{};
  std::string         fragPath = "/home/ljh/CLionProjects/VkMain/source/shader/fragment.frag";
  std::string         vertPath = "/home/ljh/CLionProjects/VkMain/source/shader/vertex.vert";
  FramebufferPool *framebufferManager;
  ImageViewManager *  imageManager;
  Swapchain *         swapchain;

  Camera *         camera;
  VulkanTexture *  texture;
  ResourceManager &resourceManager;
  MemoryAllocator &allocator;
//temp ;
  VkDeviceSize offsets = 0;
//function
  void pushConstant(VkCommandBuffer command, Material material);
};
#endif