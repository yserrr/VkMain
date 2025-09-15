#ifndef ENGINE_HPP
#define ENGINE_HPP

#include  <common.hpp>
#include  <ImporterEx.hpp>
#include  <instance.hpp>
#include  <logicalDevice.hpp>
#include  <physicalDevice.hpp>
#include  <surface.hpp>
#include  <window.hpp>
#include  <mesh.hpp>
#include  <sys_renderer.hpp>
#include  <ui_renderer.hpp>
#include  <event_manager.hpp>
#include  <asset_manager.hpp>
#include  <interface.hpp>

class Engine{
public:
  Engine() = default;
  ~Engine();
  void initialize();
  void initUI();
  void mainLoop();
  void run(const char *vert, const char *frag);

private:
//init context
  void initVulkan();
  void initFrame();
  void initRender();
  void initAsset();
  void createSwapchain();
  void createImageManager();
  void createRenderPass();
  void createFrameBuffer();
  void createCommandBuffer();
  void createSignals();
  void createDescriptors();
//draw context
  void setUp();
  void draw();
  VkCommandBuffer rec(uint32_t imageIndex);
  void summitQueue(VkCommandBuffer command, uint32_t imageIndex);

  #ifdef NDEBUG
  const bool enableValidationLayers = false;
  #else
  const bool enableValidationLayers = true;
  #endif
//handler
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
  VkPipelineLayout pipeline_layout_h;
//queue index :
  uint32_t graphics_family;
  uint32_t present_family;
  double lastFrameTime;
//signals for synchonize and context
  uint32_t currentFrame         = 0;
  uint32_t MAX_FRAMES_IN_FLIGHT = 2;
  uint32_t frameCount;
  uint64_t timeout; //similar with float
  uint32_t imageIndex;
//renderer
// 가장 낮은 의존성 (가장 먼저 생성, 가장 늦게 파괴되어야 하는 것)
  std::unique_ptr<Window> window;
  std::unique_ptr<Instance> instance;
  std::unique_ptr<Surface> surface;
  std::unique_ptr<PhysicalDevice> physicalDevice;
  std::unique_ptr<LogicalDevice> device;
  std::unique_ptr<Swapchain> swapchain;
  std::unique_ptr<ImageViewManager> imageManager;
  std::unique_ptr<RenderPassPool> renderPass;
  std::unique_ptr<FramebufferManager> frameBufferManager;
  std::unique_ptr<CommandPool> commandPool;
  std::unique_ptr<CommandBufferManager> commandBufferManager;
  std::unique_ptr<DescriptorManager> descriptorManager;

// Render / Asset / UI / Interaction (device가 생성된 후 의존하는 객체)
  std::unique_ptr<MemoryAllocator> allocator;
  std::unique_ptr<AssetManager> asset;
  std::unique_ptr<EventManager> interaction;
  std::unique_ptr<UIRenderer> uiRenderer;
  std::unique_ptr<SysRenderer> sceneRenderer;

// 동기화 객체 (device 이후 생성)
  std::unique_ptr<SemaphorePool> imageAvailableSemaphores;
  std::unique_ptr<SemaphorePool> renderFinishedSemaphores;
  std::unique_ptr<FenceManager> inFlightFences;

// 기타
  std::vector<VkRenderPassBeginInfo> renderPassInfos;
  std::array<VkClearValue, 2> clearValues;
  std::array<VkDescriptorSet, 3> sets;

  VkExtent2D extent = {1200, 800};
  const char *title = "vulkan";
};

#endif //engine_hpp