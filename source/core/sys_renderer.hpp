#include <common.hpp>
#include <pipeline.hpp>
#include <renderpass.hpp>
#include <descriptorPoolManager.hpp>
#include  <commandBuffer.hpp>
#include  <command_pool.hpp>
#include  <framebuffer.hpp>
#include <../resource/shader_module.hpp>
#include  <logicalDevice.hpp>
#include  <imageView.hpp>
#include  <swapChain.hpp>
#include  <surface.hpp>
#include  <mesh.hpp>
#include  <sampler.hpp>
#include  <texture.hpp>
#include  <asset_manager.hpp>
#include  <semaphore.hpp>
#include  <fence.hpp>
//asset manager<> renderer
//class create sturcture
struct Program{
  const char *vert;
  const char *frag;
};

//renderer create structure
struct rendererCreateInfo{
  GLFWwindow *window;
  LogicalDevice *logicalDevice;
  MemoryAllocator *allocator;
  VkExtent2D extent;
  AssetManager *asset;
  Swapchain *swapchain;
  ImageViewManager *imageManager;
  FramebufferManager *frameManager;
  VkRenderPass renderPass;
};

//class renderer
class SysRenderer{
public:
//init for rendering
  SysRenderer(rendererCreateInfo info)
    : device_h(info.logicalDevice->get()),
      physical_device_h(info.logicalDevice->getPhysicalDevice()),
      graphics_q(info.logicalDevice->getGraphicsQueue()),
      present_q(info.logicalDevice->getPresentQueue()),
      graphics_family(info.logicalDevice->getGraphicsFamily()),
      present_family(info.logicalDevice->getPresentFamily()),
      surface_h(info.logicalDevice->getSurface()),
      renderpass_h(info.renderPass),
      framebufferManager(info.frameManager),
      swapchain(info.swapchain),
      imageManager(info.imageManager),
      allocator(*info.allocator),
      asset(*info.asset) {}

  ~SysRenderer() {}

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

//rendering commander
//render pipeline
  std::unique_ptr<ShaderModule> fragShader;
  std::unique_ptr<ShaderModule> vertShader;
  std::unique_ptr<GraphicsPipeline> pipeline;
//Asset interaction reference

  FramebufferManager *framebufferManager;
  ImageViewManager *imageManager;
  Swapchain *swapchain;

  Camera *camera;
  Texture *texture;
  AssetManager &asset;
  MemoryAllocator &allocator;
//temp ;
  VkDeviceSize offsets = 0;
//function
  void pushConstant(VkCommandBuffer command, Material material);
};