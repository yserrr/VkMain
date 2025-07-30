#include<Engine.hpp> 
//
Engine::~Engine(){
    if (device ->get()  != VK_NULL_HANDLE) vkDestroyDevice    (device ->get(),  nullptr);
    if (surface->get()  != VK_NULL_HANDLE) vkDestroySurfaceKHR(instance->get(), surface->get(), nullptr);
    if (instance->get() != VK_NULL_HANDLE) vkDestroyInstance  (instance->get(), nullptr);
}

void Engine::initialize(){
    initVulkan();
    initAsset ();
    initFrame ();
    initRender();
    initUI();
    Camera* cam = asset->getCamera();
    sceneRenderer ->setCamera(cam); 
    interaction   ->setCamera(cam); 
    asset         ->uploadDescriptors(sets);
}
//vulkan resource
//get handler only
void Engine::initVulkan(){
window            = std::make_unique<Window>(extent,title);
window_h          = window->get();
instance          = std::make_unique<Instance>();  
instance_h        = instance->get();
surface           = std::make_unique<Surface>(instance_h, window_h);
surface_h         = surface->get();
physicalDevice    = std::make_unique<PhysicalDevice>(instance_h,surface_h);
physical_device_h = physicalDevice->get();
device            = std::make_unique<LogicalDevice> (physical_device_h, surface_h);
device_h          = device->get();
graphics_family   = device->getGraphicsFamily(); 
present_family    = device->getPresentFamily(); 
graphics_q        = device->getGraphicsQueue(); 
present_q         = device->getPresentQueue();
allocator         = std::make_unique<MemoryAllocator> (physical_device_h, device_h);
interaction       = std::make_unique<Interaction>     (window_h);
}

//asset and renderer allcoate
void Engine::initAsset(){
assetCreateInfo assetInfo; 
assetInfo.device    = device_h;     
assetInfo.allocator = allocator.get(); 
//asset 생성 
asset = std::make_unique<AssetManager>  (assetInfo); 
asset ->setCamera(extent);
asset ->setTexture(); 
asset ->setLight();
}

void Engine::initFrame(){ 
createSwapchain();
createImageManager();
createCommandBuffer();
createRenderPass();
createFrameBuffer();
createDescriptors();
createSignals();
}

void Engine::createSwapchain(){ 
//screate swapchain from logical device context
//swapchain setting 
SwapchainCreateInfo swapchainInfo{}; 
swapchainInfo.device         = device_h;
swapchainInfo.physicalDevice = physical_device_h; 
swapchainInfo.surface        = surface_h; 
swapchainInfo.graphicsFamily = device->getGraphicsFamily(); 
swapchainInfo.presentFamily  = device->getPresentFamily(); 
swapchainInfo.windowExtent   = extent;
swapchainInfo.allocator      = allocator.get();
swapchain     = std::make_unique<Swapchain> (swapchainInfo);
swapchain_h   = swapchain->get();
imageformat_h = swapchain->getFormat();
frameCount    = swapchain->getImages().size();
renderPassInfos.resize(frameCount);
spdlog::info("create swapchain");
}

//swapchain  can be deleted 
//but images shoud exist so sperate image manager and swapchain
void Engine::createImageManager(){ 
imageManagerCreateInfo imageManagerInfo{};
imageManagerInfo.device    =   device_h; 
imageManagerInfo.images    = &(swapchain->getImages()); 
imageManagerInfo.format    =   imageformat_h;
imageManagerInfo.extent    =   extent;
imageManagerInfo.allocator =   allocator.get();
imageManager =std::make_unique<ImageViewManager>(imageManagerInfo);
spdlog::info("create Image Manager");
}

void Engine::createCommandBuffer(){ 
//createCommandPool
commandPoolCreateInfo commandPoolInfo;
commandPoolInfo.device           = device_h;
commandPoolInfo.queueFamilyIndex = graphics_family;
commandPool                      = std::make_unique<CommandPool>(commandPoolInfo);
command_pool_h = commandPool->get();
//createCommandBuffer
commandManagerCreateInfo commandManagerInfo;
commandManagerInfo.device      = device_h; 
commandManagerInfo.commandPool = command_pool_h;
commandManagerInfo.frameCount  = frameCount; 
commandBufferManager= std::make_unique<CommandBufferManager>(commandManagerInfo);
spdlog::info("create command buffers");
}

//create renderPass
void Engine::createRenderPass(){
renderPassCreateInfo renderpassInfo;
renderpassInfo.device      = device_h;
renderpassInfo.colorFormat = imageformat_h;
renderPass =std::make_unique<RenderPass>  (renderpassInfo);
renderpass_h = renderPass->get();
spdlog::info("create RenderPass");
}

void Engine::createFrameBuffer(){ 
//create framebuffer
frameBufferCreateInfo      frameBufferInfo{};
frameBufferInfo.device     = device_h; 
frameBufferInfo.renderPass = renderpass_h;
frameBufferInfo.imageViews = &(imageManager->getImageViews());
frameBufferInfo.depthViews = &(imageManager->getDepthViews());
frameBufferInfo.extent     = extent;
frameBufferManager         =std::make_unique<FramebufferManager>(frameBufferInfo);
spdlog::info("create Frame buffers");
}


void Engine::createSignals(){ 
//create Synchro Object
signalCreateInfo signalInfo; 
signalInfo.device               = device_h; 
signalInfo.MAX_FRAMES_IN_FLIGHT = MAX_FRAMES_IN_FLIGHT;
imageAvailableSemaphores = std::make_unique<SemaphoreManager>(signalInfo);
renderFinishedSemaphores = std::make_unique<SemaphoreManager>(signalInfo);
inFlightFences           = std::make_unique<FenceManager>    (signalInfo,true);
spdlog::info("create fence and semaphore");    
}

void Engine:: createDescriptors(){

descriptorManager = std::make_unique<DescriptorManager>(device_h);
descriptorManager ->allocateDescriptor();
spdlog::info("allocate descriptors");
sets =descriptorManager->getSets();    
}

void Engine::initRender(){
spdlog::info("init renderer");
rendererCreateInfo renderinfo{};
renderinfo.window           = window_h; 
renderinfo.logicalDevice    = device.get();
renderinfo.allocator        = allocator.get();
renderinfo.extent           = extent;
renderinfo.asset            = asset.get();
renderinfo.swapchain        = swapchain.get(); 
renderinfo.imageManager     = imageManager.get(); 
renderinfo.renderPass       = renderpass_h;
Program program             = {vert, frag};   //renderer 생성
sceneRenderer = std::make_unique<SceneRenderer> (renderinfo, program);
sceneRenderer ->createPipeline((descriptorManager->getLayouts()));
pipeline_layout_h = sceneRenderer ->getPipelineLayout();
}

void Engine::initUI(){ 
//class info 분리 ->ui로 따로 만들고 ui 안에 interaction class를 넘기기
//UI RENDERER 관련 작업 진행하기 
UIRendererCreateInfo UIinfo;
UIinfo.window_h          = window_h;
UIinfo.instance_h        = instance_h;
UIinfo.device_h          = device_h;
UIinfo.physical_device_h = physical_device_h;
UIinfo.renderpass_h      = renderpass_h;
UIinfo.graphics_family   = graphics_family;
UIinfo.present_family    = present_family;
UIinfo.graphics_q        = graphics_q;
uiRenderer = std::make_unique<UIRenderer> (UIinfo);
}
