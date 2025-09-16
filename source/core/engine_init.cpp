#include<engine.hpp>
//
Engine::~Engine()
{
  if (device_h != VK_NULL_HANDLE) vkDestroyDevice(device_h, nullptr);
  if (surface_h != VK_NULL_HANDLE) vkDestroySurfaceKHR(instance_h, surface_h, nullptr);
  if (instance_h != VK_NULL_HANDLE) vkDestroyInstance(instance_h, nullptr);
}

void Engine::initialize()
{
  initVulkan();
  initAsset();
  initFrame();
  initRender();
  initUI();
  Camera *cam = asset->getCamera();
  sceneRenderer->setCamera(cam);
  interaction->setCamera(cam);
  asset->uploadDescriptors(sets);
}

void Engine::initAsset()
{
  assetCreateInfo assetInfo;
  assetInfo.device    = device_h;
  assetInfo.allocator = allocator.get();
  asset               = std::make_unique<AssetManager>(assetInfo);
  asset->setCamera(extent);
  asset->setTexture();
  asset->setLight();
}

void Engine::initFrame()
{
  createSwapchain();
  createImageManager();
  createCommandBuffer();
  createRenderPass();
  createFrameBuffer();
  createDescriptors();
  createSignals();
}

void Engine::createSwapchain()
{
//screate swapchain from logical device context
//swapchain setting
  SwapchainCreateInfo swapchainInfo{};
  swapchainInfo.device         = device_h;
  swapchainInfo.physicalDevice = physical_device_h;
  swapchainInfo.surface        = surface_h;
  swapchainInfo.graphicsFamily = graphics_family;
  swapchainInfo.presentFamily  = present_family;
  swapchainInfo.windowExtent   = extent;
  swapchainInfo.allocator      = allocator.get();
  swapchain                    = std::make_unique<Swapchain>(swapchainInfo);
  swapchain_h                  = swapchain->get();
  imageformat_h                = swapchain->getFormat();
  frameCount                   = swapchain->getImages().size();
  renderPassInfos.resize(frameCount);
  spdlog::info("create swapchain");
}

//swapchain  can be deleted
//but images shoud exist so sperate image manager and swapchain
void Engine::createImageManager()
{
  imageManagerCreateInfo imageManagerInfo{};
  imageManagerInfo.device    = device_h;
  imageManagerInfo.images    = &(swapchain->getImages());
  imageManagerInfo.format    = imageformat_h;
  imageManagerInfo.extent    = extent;
  imageManagerInfo.allocator = allocator.get();
  imageManager               = std::make_unique<ImageViewManager>(imageManagerInfo);
  spdlog::info("create Image Manager");
}

void Engine::createCommandBuffer()
{
//createCommandPool
  CommandPoolCreateInfo commandPoolInfo;
  commandPoolInfo.device           = device_h;
  commandPoolInfo.queueFamilyIndex = graphics_family;
  commandPool                      = std::make_unique<CommandPool>(commandPoolInfo);
  command_pool_h                   = commandPool->get();
//createCommandBuffer
  commandManagerCreateInfo commandManagerInfo;
  commandManagerInfo.device      = device_h;
  commandManagerInfo.commandPool = command_pool_h;
  commandManagerInfo.frameCount  = frameCount;
  commandBufferManager           = std::make_unique<CommandBufferManager>(commandManagerInfo);
  spdlog::info("create command buffers");
}

//create renderPass
void Engine::createRenderPass()
{
  RenderPassCreateInfo renderpassInfo;
  renderpassInfo.device      = device_h;
  renderpassInfo.colorFormat = imageformat_h;
  renderPass                 = std::make_unique<RenderPassPool>(renderpassInfo);
  renderpass_h               = renderPass->buildForwardPass();
  spdlog::info("create RenderPass");
}

void Engine::createFrameBuffer()
{
//create framebuffer
  frameBufferCreateInfo frameBufferInfo{};
  frameBufferInfo.device     = device_h;
  frameBufferInfo.renderPass = renderpass_h;
  frameBufferInfo.imageViews = &(imageManager->getImageViews());
  frameBufferInfo.depthViews = &(imageManager->getDepthViews());
  frameBufferInfo.extent     = extent;
  frameBufferManager         = std::make_unique<FramebufferManager>(frameBufferInfo);
  spdlog::info("create Frame buffers");
}

void Engine::createSignals()
{
//create Synchro Object
  SignalCreateInfo signalInfo;
  signalInfo.device               = device_h;
  signalInfo.MAX_FRAMES_IN_FLIGHT = MAX_FRAMES_IN_FLIGHT;
  imageAvailableSemaphores        = std::make_unique<SemaphorePool>(signalInfo);
  renderFinishedSemaphores        = std::make_unique<SemaphorePool>(signalInfo);
  inFlightFences                  = std::make_unique<FencePool>(signalInfo, true);
  spdlog::info("create fence and semaphore");
}

void Engine::createDescriptors()
{
  descriptorManager = std::make_unique<DescriptorManager>(device_h);
  descriptorManager->allocateDescriptor();
  spdlog::info("allocate descriptors");
  sets = descriptorManager->getSets();
}

void Engine::initRender()
{
  spdlog::info("init renderer");
  rendererCreateInfo renderinfo{};
  renderinfo.window       = window_h;
  renderinfo.device_h     = device_h;
  renderinfo.allocator    = allocator.get();
  renderinfo.extent       = extent;
  renderinfo.asset        = asset.get();
  renderinfo.swapchain    = swapchain.get();
  renderinfo.imageManager = imageManager.get();
  renderinfo.renderPass   = renderpass_h;
  sceneRenderer           = std::make_unique<SysRenderer>(renderinfo);
  sceneRenderer->createPipeline((descriptorManager->getLayouts()));
  pipeline_layout_h = sceneRenderer->getPipelineLayout();
}

void Engine::initUI()
{
  UIRendererCreateInfo UIinfo;
  UIinfo.window_h          = window_h;
  UIinfo.instance_h        = instance_h;
  UIinfo.device_h          = device_h;
  UIinfo.physical_device_h = physical_device_h;
  UIinfo.renderpass_h      = renderpass_h;
  UIinfo.graphics_family   = graphics_family;
  UIinfo.present_family    = present_family;
  UIinfo.graphics_q        = graphics_q;
  uiRenderer               = std::make_unique<UIRenderer>(UIinfo);
}