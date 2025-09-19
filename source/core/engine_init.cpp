#include<engine.hpp>

void Engine::initialize()
{
  allocator     = std::make_unique<MemoryAllocator>(physical_device_h, device_h);
  eventManager_ = std::make_unique<EventManager>(window_h);
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
  eventManager_->currentExtent = swapchain->getExtent();
  ViewManagerCreateInfo imageManagerInfo{};
  imageManagerInfo.device    = device_h;
  imageManagerInfo.images    = &(swapchain->getImages());
  imageManagerInfo.format    = imageformat_h;
  imageManagerInfo.extent    = extent;
  imageManagerInfo.allocator = allocator.get();
  imageManager               = std::make_unique<SwapchainViewManager>(imageManagerInfo);
  spdlog::info("create Image Manager");

  //createCommandPool
  CommandPoolCreateInfo commandPoolInfo;
  commandPoolInfo.device           = device_h;
  commandPoolInfo.queueFamilyIndex = graphics_family;
  commandPool                      = std::make_unique<CommandPool>(commandPoolInfo);
  command_pool_h                   = commandPool->get();
  //createCommandBuffer
  CommandPoolManagerCreateInfo commandManagerInfo;
  commandManagerInfo.device      = device_h;
  commandManagerInfo.commandPool = command_pool_h;
  commandManagerInfo.frameCount  = frameCount;
  commandBufferManager           = std::make_unique<CommandPoolManager>(commandManagerInfo);
  spdlog::info("create command buffers");

  RenderPassPoolCreateInfo renderpassInfo;
  renderpassInfo.device      = device_h;
  renderpassInfo.colorFormat = imageformat_h;
  renderPass                 = std::make_unique<RenderPassPool>(renderpassInfo);
  renderpass_h               = renderPass->buildForwardPass();
  spdlog::info("create RenderPass");

  FramePoolCreateInfo frameBufferInfo{};
  frameBufferInfo.device     = device_h;
  frameBufferInfo.renderPass = renderpass_h;
  frameBufferInfo.imageViews = &(imageManager->getImageViews());
  frameBufferInfo.depthViews = &(imageManager->getDepthViews());
  frameBufferInfo.extent     = extent;
  frameBufferManager         = std::make_unique<FramebufferPool>(frameBufferInfo);
  spdlog::info("create Frame buffers");

  spdlog::info("allocate descriptors");

  SignalCreateInfo signalInfo;
  signalInfo.device               = device_h;
  signalInfo.MAX_FRAMES_IN_FLIGHT = MAX_FRAMES_IN_FLIGHT;
  imageAvailableSemaphores        = std::make_unique<SemaphorePool>(signalInfo);
  renderFinishedSemaphores        = std::make_unique<SemaphorePool>(signalInfo);
  inFlightFences                  = std::make_unique<FencePool>(signalInfo, true);
  spdlog::info("create fence and semaphore");

  ResourceManagerCreateInfo resource_manager_create_info;
  resource_manager_create_info.device    = device_h;
  resource_manager_create_info.allocator = allocator.get();

  resourceManager_ = std::make_unique<ResourceManager>(resource_manager_create_info);
  resourceManager_->setTexture();

  spdlog::info("init renderer");
  RendererCreateInfo renderinfo{};
  renderinfo.window       = window_h;
  renderinfo.device_h     = device_h;
  renderinfo.allocator    = allocator.get();
  renderinfo.extent       = extent;
  renderinfo.asset        = resourceManager_.get();
  renderinfo.swapchain    = swapchain.get();
  renderinfo.imageManager = imageManager.get();
  renderinfo.renderPass   = renderpass_h;
  sceneRenderer           = std::make_unique<SceneRenderer>(renderinfo);
  sceneRenderer->createPipeline((resourceManager_->descriptorManager->getLayouts()));

  pipeline_layout_h = sceneRenderer->getPipelineLayout();

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
  uiRenderer->setResourceManager(resourceManager_.get());

  Camera *cam = resourceManager_->getCamera();
  sceneRenderer->setCamera(cam);
  eventManager_->setCamera(cam);
  eventManager_->resourcesManager_ = resourceManager_.get();

  resourceManager_->uploadDescriptors();

  eventManager_->setRenderer(sceneRenderer.get());
}