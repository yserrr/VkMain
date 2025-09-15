#include "test_module.hpp"
#include "ui_controller.hpp"
#include "../resources/temp/descriptor_set_tracker.hpp"
#include <base_geometry.hpp>

inline VkDescriptorSetLayout uptrToVkLayout(uintptr_t uptr)
{
  return reinterpret_cast<VkDescriptorSetLayout>(uptr);
}

inline VkDescriptorSet uptrToVk(uintptr_t uptr)
{
  return reinterpret_cast<VkDescriptorSet>(uptr);
}

int main()
{
  InstanceCreateInfo info;
  info.applicationName    = "myEngine";
  info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  info.apiVersion         = VK_API_VERSION_1_3;
  info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
  info.engineName         = "noEngine";
  info.enableValidation   = true;
  VulkanDevice device(info);
  const VkContext& context = device.getContext();

  VkPhysicalDeviceProperties physicalDeviceProperties;
  vkGetPhysicalDeviceProperties(context.physical_device_h, &physicalDeviceProperties);
  auto minUboAlign = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
  MemoryAllocator allocator(context.physical_device_h, context.device_h);
  SwapchainCreateInfo swapchainInfo(allocator);
  swapchainInfo.device_h         = context.device_h;
  swapchainInfo.physicalDevice_h = context.physical_device_h;
  swapchainInfo.surface_h        = context.surface_h;
  swapchainInfo.graphicsFamily_h = context.graphics_family;
  swapchainInfo.presentFamily_h  = context.present_family;
  swapchainInfo.windowExtent     = device.getExtent();
  SwapchainManager swapchain(swapchainInfo);
  auto device_h = context.device_h;
  ImporterEx importer;
  ImportResult data = importer.loadScene(
                                         "/home/ljh/바탕화면/vk/extern/externModel/35-rp_sophia_animated_003_idling_fbx/rp_sophia_animated_003_idling.fbx");
  ShaderModule vertshader(device_h);
  ShaderModule fragshader(device_h);
  ShaderModule compshader(device_h);
  std::string vertPath = "/home/ljh/바탕화면/vk/source/shader/vert.vert";
  std::string fragPath = "/home/ljh/바탕화면/vk/source/shader/simple_light_frag.frag";
  //std::string compPath = "/home/ljh/바탕화면/vk/source/shader/sculpt_compute.comp"
  vertshader.setShader(vertPath, shaderc_vertex_shader);
  fragshader.setShader(fragPath, shaderc_fragment_shader);
  CommandPoolCreateInfo commandPoolInfo;
  commandPoolInfo.device           = device_h;
  commandPoolInfo.queueFamilyIndex = context.graphics_family;
  CommandPool commandPool(commandPoolInfo);
  CommandManagerCreateInfo commandManagerInfo{};
  commandManagerInfo.device_h    = device_h;
  commandManagerInfo.commandPool = commandPool.get();
  commandManagerInfo.frameCount  = swapchain.getImageCount();
  CommandBufferManager commandBufferManager(commandManagerInfo);
  spdlog::info("create command buffers");
  RenderPassCreateInfo render_pass_create_info;
  render_pass_create_info.device      = device_h;
  render_pass_create_info.colorFormat = swapchain.getFormat();
  RenderPassPool render_pass(render_pass_create_info);

  VkRenderPass renderPass = render_pass.getRenderPass();
  swapchain.setRenderPass(renderPass);
  swapchain.createFrame();

  uint32_t dsl_layout_allocate_size = 50;
  DescriptorTracker tracker(dsl_layout_allocate_size);
  gpu::descriptor_layout_state cam_layout     = tracker.allocate();
  gpu::descriptor_layout_state light_layout   = tracker.allocate();
  gpu::descriptor_layout_state texture_layout = tracker.allocate();
  ///write layout only based on allocate
  ///only descriptor tracker own static info
  DescriptorLayoutPool dsl(device_h);
  dsl.writeDynamicUboLayout(cam_layout);
  dsl.writeBindlessTextureLayout(texture_layout);
  dsl.writeDefaultLayout(light_layout);

  auto cam_dsl      = uptrToVkLayout(cam_layout.layout_ptr);
  auto bindless_dsl = uptrToVkLayout(texture_layout.layout_ptr);
  auto light_dsl    = uptrToVkLayout(light_layout.layout_ptr);

  /// todo:
  ///  check module on layout writing
  ///  descriptor Pool only get the d
  std::vector<VkDescriptorSetLayout> dsls = {cam_dsl, bindless_dsl, light_dsl};
  PipelineCreateInfo plCi(dsls);
  plCi.renderPass       = renderPass;
  plCi.extent           = swapchain.getExtent();
  plCi.vertShaderModule = vertshader.get();
  plCi.fragShaderModule = fragshader.get();
  plCi.device           = device_h;
  PipelinePool pipelinePool(plCi);
  //pipelinePool.createComputePipeline(setLayouts, compshader.get());
  uint32_t currentFrame         = 0;
  uint32_t MAX_FRAMES_IN_FLIGHT = 2;
  uint32_t imageIndex           = 0;
  VkPipeline currnetPipeline    = VK_NULL_HANDLE;

  SignalCreateInfo sigCi{};
  sigCi.device               = device_h;
  sigCi.MAX_FRAMES_IN_FLIGHT = MAX_FRAMES_IN_FLIGHT;
  bool first                 = true;
  VkViewport viewport{};
  VkRect2D scissor{};
  FencePool fence(sigCi, first);
  SemaphorePool semaphore_manager(sigCi);
  SemaphorePool subQueue(sigCi);
  imageIndex = swapchain.getImageCount();
  std::array<VkClearValue, 2> clearValues{};
  std::vector<VkRenderPassBeginInfo> renderPassInfos(MAX_FRAMES_IN_FLIGHT);
  DescriptorTracker descriptorTracker(5);
  auto defaultLayout = descriptorTracker.allocate();

  DescriptorLayoutPool descriptorLayout(device_h);
  descriptorLayout.writeDefaultLayout(defaultLayout);
  VkDescriptorSetLayout defaultLayout_ = uptrToVkLayout(defaultLayout.layout_ptr);

  DescriptorSetUploader descriptorSet(device_h);
  descriptorSet.createPool(defaultLayout_, gpu::descriptor_usage::BASE);
  gpu::descriptor_set_state camSetState      = descriptorSet.allocate(gpu::descriptor_usage::BASE);
  gpu::descriptor_set_state bindlessSetStaet = descriptorSet.allocate(gpu::descriptor_usage::BASE);
  gpu::descriptor_set_state lightSetStae     = descriptorSet.allocate(gpu::descriptor_usage::BASE);
  gpu::descriptor_set_state materialState    = descriptorSet.allocate(gpu::descriptor_usage::BASE);
  MeshDesc desc;
  MeshManager mesh;


  mesh.setMesh(data.meshes);
  auto cmd = mesh.getDrawCmd();
  BufferPool vBuf(allocator, BufferType::VERTEX, cmd.vBufferSize);
  vBuf.create();
  IndexBuffer iBuf(allocator, BufferType::INDEX, cmd.iBufferSize);
  iBuf.create();
  VkDeviceSize capacity = cmd.vBufferSize + cmd.iBufferSize;

  std::shared_ptr<streamingBuffer> staging_pool = std::make_shared<streamingBuffer>(allocator, capacity * 2);
  staging_pool->setAlign(physicalDeviceProperties);
  staging_pool->setCoherentFlag(true);
  staging_pool->create();
  auto vBlock = staging_pool->acquire(cmd.vBufferSize, sizeof(VertexAll));
  auto iBlock = staging_pool->acquire(cmd.iBufferSize, 4);
  staging_pool->map(cmd.vData, vBlock.offset, cmd.vBufferSize);
  staging_pool->map(cmd.iData, iBlock.offset, iBlock.size);
  staging_pool->flush(vBlock);
  staging_pool->flush(iBlock);

  VkDescriptorSet camSet      = uptrToVk(camSetState.descriptor_set);
  VkDescriptorSet bindlessSet = uptrToVk(bindlessSetStaet.descriptor_set);
  VkDescriptorSet light_set   = uptrToVk(lightSetStae.descriptor_set);

  VkPhysicalDeviceMemoryProperties memProps;
  vkGetPhysicalDeviceMemoryProperties(context.physical_device_h, &memProps);


  VkPhysicalDeviceProperties deviceProps;
  vkGetPhysicalDeviceProperties(context.physical_device_h, &deviceProps);

  VkDeviceSize alignment = deviceProps.limits.minMemoryMapAlignment;
  spdlog::info("minMemoryMapAlignment = {}", alignment);

  BaseCamera cam;
  //cam.setDesc(data.cameras);
  auto camDesc          = cam.getDesc();
  VkDeviceSize camAlign = camDesc.stride;

  camAlign                = (camAlign + minUboAlign - 1) & ~(minUboAlign - 1);
  VkDeviceSize camBufSize = camAlign * (camDesc.camMax);
  CameraBuffer camBuf(allocator,
                      BufferType::UNIFORM,
                      camBufSize,
                      AccessPolicy::HostPreferred);
  camBuf.create();
  camBuf.map();
  //upload buffer

  std::vector<VkDescriptorBufferInfo> camBufInfo(camDesc.camMax);
  for (auto& Ci : camBufInfo)
  {
    Ci.buffer = camBuf.getBuffer();
    Ci.range  = camDesc.stride;
    Ci.offset = 0;
  }
  VkWriteDescriptorSet descriptorWrite{};
  if (cam.isDirty())
  {
    const auto& bat = cam.flushBatch();
    for (auto& camCmd : bat.cmd)
    {
      //mapping seperated switch
      //min stride change, -> stride change
      ///todo:
      /// set:
      /// element
      /// count
      /// type
      /// offset
      /// if need, dynamic upload
      /// just ubo update , but Multi veiw mode, upload with data set
      camBuf.upload(&camCmd.first, camAlign, camCmd.second * camAlign);
      assert(camAlign % minUboAlign == 0);
      assert((camCmd.second * camAlign) % minUboAlign== 0);
    }
    descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet          = camSet;
    descriptorWrite.dstBinding      = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptorWrite.descriptorCount = 8;
    descriptorWrite.pBufferInfo     = camBufInfo.data();
    vkUpdateDescriptorSets(device_h, 1, &descriptorWrite, 0, nullptr);
  }
  TextureManager texture;
  TextureDesc txtdesc;
  txtdesc.embeddedIndex = -1;
  txtdesc.path          =
  "/home/ljh/바탕화면/vk/extern/externModel/35-rp_sophia_animated_003_idling_fbx/texture/rp_sophia_animated_003_dif.jpg";
  txtdesc.flag = TexUsage::ALBEDO;
  texture.setImage(data.textures, false, false);
  texture.setImage(txtdesc, false, false);
  auto textureBat = texture.flush();

  Sampler sampler(device_h);
  ImageCreateInfo imgCi(allocator);
  imgCi.device          = device_h;
  imgCi.sampler         = sampler.get();
  imgCi.bindlessSet     = bindlessSet;
  imgCi.enableBindless  = true;
  imgCi.bindlessBinding = 0; //bindiness layout binding = 0

  ImageManager img(imgCi);
  img.setStagingPool(staging_pool);
  img.createImage(textureBat.img, textureBat.view);


  auto imgNode = img.flush();


  VkDeviceSize lightUboSize = sizeof(gpu::light_batch);
  lightUboSize              = (lightUboSize + minUboAlign - 1) & ~(minUboAlign - 1);

  LightBuffer lightBuf = BufferPool(allocator, BufferType::UNIFORM, lightUboSize);
  lightBuf.create();
  lightBuf.map();

  LightManager light_manager;
  light_manager.setColor(glm::vec3(0, 0, 1));
  light_manager.setIntensity(3.0f);
  light_manager.setPosition(glm::vec3(100, 100, 100));
  light_manager.buildLight();
  light_manager.setColor(glm::vec3(0, 0, 1));
  light_manager.setIntensity(0.0f);
  light_manager.setPosition(glm::vec3(100, 100, 100));
  light_manager.buildLight();
  light_manager.setColor(glm::vec3(0, 1, 1));
  light_manager.setIntensity(1000.0f);
  light_manager.setPosition(glm::vec3(100, 100, 100));
  light_manager.buildLight();
  light_manager.setColor(glm::vec3(1, 1, 1));
  light_manager.setIntensity(100.0f);
  light_manager.setPosition(glm::vec3(100, 100, 100));
  light_manager.buildLight();
  light_manager.setColor(glm::vec3(1, 1, 1));
  light_manager.setIntensity(1.0f);
  light_manager.setPosition(glm::vec3(100, 100, 100));
  light_manager.buildLight();
  gpu::light_batch light_data = light_manager.flush();
  lightBuf.upload(&light_data, sizeof(gpu::light_batch));


  ControllerCreateInfo obsCI{cam, light_manager, texture, mesh};
  UIControler controler(obsCI);

  VkDescriptorBufferInfo bufferInfoUbo{};
  bufferInfoUbo.buffer = lightBuf.getBuffer();
  bufferInfoUbo.offset = 0;
  bufferInfoUbo.range  = sizeof(gpu::light_batch);

  VkWriteDescriptorSet writeUbo{};
  writeUbo.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeUbo.dstSet          = light_set;
  writeUbo.dstBinding      = 0;
  writeUbo.dstArrayElement = 0;
  writeUbo.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writeUbo.descriptorCount = 1;
  writeUbo.pBufferInfo     = &bufferInfoUbo;
  vkUpdateDescriptorSets(device_h, 1, &writeUbo, 0, nullptr);
  ///todo: manager <> descritporset tracker setup
  /// here -> command
  /// before cmd, setup the front resource
  VkSingleCommandBuffer vk_cmd = commandBufferManager.beginSingleTime();

  VkBufferCopy v_region{};
  v_region.srcOffset = vBlock.offset;
  v_region.dstOffset = 0;
  v_region.size      = vBlock.size;
  vkCmdCopyBuffer(vk_cmd, vBlock.buffer, vBuf.getBuffer(), 1, &v_region);

  VkBufferCopy i_region{};
  i_region.srcOffset = iBlock.offset;
  i_region.dstOffset = 0;
  i_region.size      = iBlock.size;
  vkCmdCopyBuffer(vk_cmd, iBlock.buffer, iBuf.getBuffer(), 1, &i_region);

  VkBufferMemoryBarrier v_barrier{};
  v_barrier.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  v_barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
  v_barrier.dstAccessMask       = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
  v_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  v_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  v_barrier.buffer              = vBuf.getBuffer();
  v_barrier.offset              = 0;
  v_barrier.size                = VK_WHOLE_SIZE;

  uint32_t dynamicOffsets[8];

  for (uint32_t i = 0; i < camDesc.camMax; ++i)
  {
    dynamicOffsets[i] = i * camAlign; // alignedCameraSize = stride 맞춘 UBO 크기
  }

  vkCmdPipelineBarrier(
                       vk_cmd,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                       0,
                       0,
                       nullptr,
                       1,
                       &v_barrier,
                       0,
                       nullptr
                      );

  ///index buffer memory:
  ///srcAccess Mask: write -> read ->pipeline masking structure
  ///

  VkBufferMemoryBarrier i_barrier{};
  i_barrier.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  i_barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
  i_barrier.dstAccessMask       = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
  i_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  i_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  i_barrier.buffer              = iBuf.getBuffer();
  i_barrier.offset              = 0;
  i_barrier.size                = VK_WHOLE_SIZE;

  vkCmdPipelineBarrier(
                       vk_cmd,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                       0,
                       0,
                       nullptr,
                       1,
                       &i_barrier,
                       0,
                       nullptr
                      );


  VkPipelineStageFlags sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;


  auto txtlayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  for (auto& tmp : imgNode)
  {
    // 1. UNDEFINED -> TRANSFER_DST_OPTIMAL

    VkImageMemoryBarrier toTransfer{};
    toTransfer.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    toTransfer.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    toTransfer.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    toTransfer.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    toTransfer.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    toTransfer.image                           = tmp.img; // images_[index]
    toTransfer.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    toTransfer.subresourceRange.baseMipLevel   = 0;
    toTransfer.subresourceRange.levelCount     = 1;
    toTransfer.subresourceRange.baseArrayLayer = 0;
    toTransfer.subresourceRange.layerCount     = 1;
    toTransfer.srcAccessMask                   = 0;
    toTransfer.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(
                         vk_cmd,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         // 또는 HOST_BIT
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &toTransfer
                        );

    vkCmdCopyBufferToImage(vk_cmd, staging_pool->getBuffer(), tmp.img, txtlayout, 1, &tmp.regions);

    vkCmdPipelineBarrier(
                         vk_cmd,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &tmp.barrier2
                        );
  }


  VkDeviceSize offsets[] = {0}; // 오프셋이 0인 배열
  auto rrr               = vkEndCommandBuffer(vk_cmd);
  assert(rrr == VK_SUCCESS);
  VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
  si.commandBufferCount = 1;
  si.pCommandBuffers    = &vk_cmd;
  vkQueueSubmit(context.graphics_q, 1, &si, VK_NULL_HANDLE);

  vkQueueWaitIdle(context.graphics_q);
  spdlog::info("index size: {}", cmd.indexCnt);
  gpu::cam_batch batch;

  EventManager interaction(context.window_h);
  currnetPipeline = pipelinePool.getPipeline(interaction.getPolygonMode());
  interaction.setCamera(&cam);
  interaction.setSwapchain(&swapchain);

  UiCreateInfo uiCi{};
  uiCi.device_h          = device_h;
  uiCi.graphics_family   = context.graphics_family;
  uiCi.graphics_q        = context.graphics_q;
  uiCi.window_h          = context.window_h;
  uiCi.physical_device_h = context.physical_device_h;
  uiCi.instance_h        = context.instance_h;
  uiCi.renderpass_h      = renderPass;
  uiCi.present_family    = context.present_family;

  UIRenderer ui(uiCi);

  for (auto& tmp : imgNode)
  {
    spdlog::info("idx:: {}", tmp.index);
    img.writeDescriptorSet(tmp.index);
  }

  while (!glfwWindowShouldClose(context.window_h))
  {
    glfwPollEvents();
    if (cam.isDirty())
    {
      batch = cam.flushBatch();
      if (batch.multiView == MultiView::SINGLE)
      {
        //cam.debug();
        //spdlog::info("current's index: {} ", batch.current_cam);
        camBuf.upload(&batch.cmd[batch.current_cam].first, camAlign, batch.current_cam * camAlign);
      }
    }
    interaction.getKey();
    interaction.wheelUpdate();
    fence.wait(currentFrame, 1000000); //ns
    fence.reset(currentFrame);
    //push semaphore
    VkSemaphore sig = semaphore_manager.getSingal(currentFrame);
    if (vkAcquireNextImageKHR(device_h,
                              swapchain.getSwapchain(),
                              UINT64_MAX,
                              sig,
                              VK_NULL_HANDLE,
                              &imageIndex) != VK_SUCCESS)
    {
      if (interaction.isResized())
      {
        vkDeviceWaitIdle(device_h);
        spdlog::info("resized");
        ViewExtent extent = interaction.getExtent();
        swapchain.setExtent(extent.width, extent.height);
        swapchain.recreateSwapchain();
        semaphore_manager.recreate();
        fence.recreate();
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        vkDeviceWaitIdle(device_h);
        continue;
      }
    }
    VkCommandBuffer command = commandBufferManager.beginPrimary(imageIndex);

    clearValues[0].color                            = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil                     = {1.0f, 0}; // 깊이 초기화 값
    renderPassInfos[currentFrame].sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfos[currentFrame].renderPass        = renderPass;
    renderPassInfos[currentFrame].framebuffer       = swapchain.getFrameBuffer(imageIndex);
    renderPassInfos[currentFrame].renderArea.offset = {0, 0};
    renderPassInfos[currentFrame].renderArea.extent = swapchain.getExtent();
    renderPassInfos[currentFrame].clearValueCount   = 2;
    renderPassInfos[currentFrame].pClearValues      = clearValues.data();
    /// alwas viewfort setting with dynamic drawing
    /// setting view, just draw call multiple to render multi view

    vkCmdBeginRenderPass(command, &renderPassInfos[currentFrame], VK_SUBPASS_CONTENTS_INLINE); // RenderPass 시작
    currnetPipeline = pipelinePool.getPipeline(interaction.getPolygonMode());
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, currnetPipeline); // 파이프라인 바인딩
    vkCmdSetDepthTestEnable(command, VK_TRUE);
    vkCmdSetStencilTestEnable(command, VK_FALSE);
    vkCmdSetDepthCompareOp(command, VK_COMPARE_OP_LESS);

    vkCmdBindVertexBuffers(command, 0, 1, vBuf.getBufferPtr(), offsets);
    vkCmdBindIndexBuffer(command, iBuf.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    ///todo: pipeline state settuped
    /// => update the polygon mode,
    ///    render cam target
    ///    viewport modes set scissors

    if (interaction.isMultiView())
    {
      // swapchain 크기 가져오기
      auto extent      = swapchain.getExtent();
      float halfWidth  = extent.width / 2.0f;
      float halfHeight = extent.height / 2.0f;

      vkCmdBindDescriptorSets(command,
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipelinePool.getBaseLayout(),
                              1,
                              1,
                              &bindlessSet,
                              0,
                              nullptr);

      vkCmdBindDescriptorSets(command,
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipelinePool.getBaseLayout(),
                              2,
                              1,
                              &light_set,
                              0,
                              nullptr);

      for (uint32_t i = 0; i < 4; i++)
      {
        // viewport 설정
        viewport.width    = halfWidth;
        viewport.height   = halfHeight;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        viewport.x = (i % 2) * halfWidth;  // 0 또는 W/2
        viewport.y = (i / 2) * halfHeight; // 0 또는 H/2
        vkCmdSetViewport(command, 0, 1, &viewport);

        scissor.offset = {static_cast<int32_t>(viewport.x), static_cast<int32_t>(viewport.y)};
        scissor.extent = {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)};
        vkCmdSetScissor(command, 0, 1, &scissor);
        //vkCmdDrawIndexed(command, cmd.indexCnt, 1, 0, 0, 0);
        vkCmdDraw(command, cmd.vertexCnt, 1, 0, 0);
        vkCmdBindDescriptorSets(
                                command,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelinePool.getBaseLayout(),
                                0,
                                1,
                                &camSet,
                                8,
                                &dynamicOffsets[batch.current_cam]);
      }
    }
    else
    {
      viewport.x        = 0.0f;
      viewport.y        = 0.0f;
      viewport.width    = (float)swapchain.getExtent().width;
      viewport.height   = (float)swapchain.getExtent().height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(command, 0, 1, &viewport);

      scissor.offset = {0, 0};
      scissor.extent = swapchain.getExtent();
      vkCmdSetScissor(command, 0, 1, &scissor);
      vkCmdBindDescriptorSets(
                              command,
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipelinePool.getBaseLayout(),
                              0,
                              1,
                              &camSet,
                              8,
                              dynamicOffsets);

      vkCmdBindDescriptorSets(command,
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipelinePool.getBaseLayout(),
                              1,
                              1,
                              &bindlessSet,
                              0,
                              nullptr);

      vkCmdBindDescriptorSets(command,
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipelinePool.getBaseLayout(),
                              2,
                              1,
                              &light_set,
                              0,
                              nullptr);


      //vkCmdDrawIndexed(command, cmd.indexCnt, 1, 0, 0, 0);
      vkCmdDraw(command, cmd.vertexCnt, 1, 0, 0);
    }

    vkCmdNextSubpass(command, VK_SUBPASS_CONTENTS_INLINE);
    ui.draw(command, controler);
    vkCmdEndRenderPass(command);
    commandBufferManager.end(command);
    VkSubmitInfo submitInfo{};
    submitInfo.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores        = semaphore_manager.getSingal(currentFrame);
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = &waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;
    submitInfo.commandBufferCount     = 1;
    submitInfo.pCommandBuffers        = &command;

    // 프레젠트 정보 설정
    VkSemaphore signalSemaphores[]  = {subQueue.getSingal(currentFrame)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;
    VK_ASSERT(vkQueueSubmit(context.graphics_q, 1, &submitInfo, fence.get(currentFrame)));
    VkSwapchainKHR swapchains[] = {swapchain.getSwapchain()};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = swapchains;
    presentInfo.pImageIndices      = &(imageIndex);

    if ((vkQueuePresentKHR(context.present_q, &presentInfo) != VK_SUCCESS))
    {
      if (interaction.isResized())
      {
        vkDeviceWaitIdle(device_h);
        spdlog::info("resized");
        ViewExtent extent = interaction.getExtent();
        swapchain.setExtent(extent.width, extent.height);
        swapchain.recreateSwapchain();
        semaphore_manager.recreate();
        fence.recreate();
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        vkDeviceWaitIdle(device_h);
        continue;
      }
    }
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }
  return 0;
}
