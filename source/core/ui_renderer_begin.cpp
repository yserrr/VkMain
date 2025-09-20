#include<ui_renderer.hpp>

UIRenderer::UIRenderer(UIRendererCreateInfo info)
  : device_h(info.device_h),
    instance_h(info.instance_h),
    physical_device_h(info.physical_device_h),
    window_h(info.window_h),
    renderpass_h(info.renderpass_h),
    graphics_family(info.graphics_family),
    present_family(info.present_family),
    graphics_q(info.graphics_q)
{

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  ImGui::StyleColorsDark();
  createPool();

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physical_device_h, &properties);
  ImGui_ImplVulkan_InitInfo UIinfo = {};
  UIinfo.Instance                  = instance_h;
  UIinfo.PhysicalDevice            = physical_device_h;
  UIinfo.Device                    = device_h;
  UIinfo.QueueFamily               = graphics_family;
  UIinfo.Queue                     = graphics_q;
  UIinfo.PipelineCache             = VK_NULL_HANDLE;
  UIinfo.DescriptorPool            = imguiPool;
  UIinfo.Allocator                 = nullptr;
  UIinfo.MinImageCount             = 2;
  UIinfo.ImageCount                = 3;
  UIinfo.CheckVkResultFn           = nullptr; // 필요하면 콜백 등록
  UIinfo.RenderPass                = renderpass_h;
  UIinfo.ApiVersion                = properties.apiVersion;
  UIinfo.Subpass                   = 1;
  UIinfo.UseDynamicRendering       = info.useDynamic;

  ImGui_ImplGlfw_InitForVulkan(window_h, true);
  ImGui_ImplVulkan_Init(&UIinfo);
  spdlog::info("start imgui");
  setupStyle();
}

void UIRenderer::createPool()
{
  VkDescriptorPoolSize pool_sizes[] = {
  {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
  {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
  {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
  {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
  {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
  {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
  {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
  };
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets                    = 1000 * IM_ARRAYSIZE(pool_sizes);
  pool_info.poolSizeCount              = (uint32_t) IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes                 = pool_sizes;
  if (vkCreateDescriptorPool(device_h, &pool_info, nullptr, &imguiPool) != VK_SUCCESS)
  {
    throw std::runtime_error("fail to create Imgui Pool");
  }
}

void UIRenderer::setupStyle()
{
  ImGui::StyleColorsDark(); // 다크 테마 기반
  ImGuiStyle &style         = ImGui::GetStyle();
  ImVec4 *colors            = style.Colors;
  colors[ImGuiCol_WindowBg] = ImVec4(0.010f, 0.010f, 0.010f, 1.0f);
  colors[ImGuiCol_ChildBg]  = ImVec4(0.010f, 0.010f, 0.010f, 1.0f);
  colors[ImGuiCol_PopupBg]  = ImVec4(0.010f, 0.010f, 0.010f, 1.0f);

  colors[ImGuiCol_TitleBg]          = ImVec4(0.03f, 0.03f, 0.03f, 1.0f);
  colors[ImGuiCol_TitleBgActive]    = ImVec4(0.03f, 0.03f, 0.03f, 1.0f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.03f, 0.03f, 0.03f, 1.0f);

  colors[ImGuiCol_Button]        = ImVec4(0.20f, 0.45f, 0.85f, 1.0f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.55f, 0.95f, 1.0f);
  colors[ImGuiCol_ButtonActive]  = ImVec4(0.15f, 0.35f, 0.75f, 1.0f);

  colors[ImGuiCol_FrameBg]        = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.45f, 0.75f, 1.0f);
  colors[ImGuiCol_FrameBgActive]  = ImVec4(0.20f, 0.40f, 0.70f, 1.0f);

  colors[ImGuiCol_Text]         = ImVec4(0.90f, 0.90f, 0.90f, 1.0f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.0f);

  colors[ImGuiCol_Border]       = ImVec4(0.30f, 0.30f, 0.35f, 1.0f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

  colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
  colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.30f, 0.30f, 0.35f, 1.0f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.40f, 1.0f);
  colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.40f, 0.40f, 0.45f, 1.0f);

  colors[ImGuiCol_CheckMark]        = ImVec4(0.20f, 0.45f, 0.85f, 1.0f);
  colors[ImGuiCol_SliderGrab]       = ImVec4(0.20f, 0.45f, 0.85f, 1.0f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.25f, 0.55f, 0.95f, 1.0f);

  colors[ImGuiCol_Tab]                = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
  colors[ImGuiCol_TabHovered]         = ImVec4(0.25f, 0.45f, 0.75f, 1.0f);
  colors[ImGuiCol_TabActive]          = ImVec4(0.20f, 0.40f, 0.70f, 1.0f);
  colors[ImGuiCol_TabUnfocused]       = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);

  style.WindowRounding    = 8.0f;
  style.FrameRounding     = 6.0f;
  style.ScrollbarRounding = 6.0f;
  style.GrabRounding      = 6.0f;
  style.PopupRounding     = 6.0f;

  style.WindowPadding    = ImVec2(15, 15);
  style.FramePadding     = ImVec2(10, 6);
  style.ItemSpacing      = ImVec2(10, 8);
  style.ItemInnerSpacing = ImVec2(6, 6);
  style.ScrollbarSize    = 14.0f;
  style.GrabMinSize      = 14.0f;
  style.FontSizeBase     = 11;
}

void UIRenderer::uploadBackgroundImage()
{
  backgroundTexture_ = resourceManager_->getTexture(std::string("VkVideo.png"));
  if (backgroundTexture_ == nullptr)
  {
    throw std::runtime_error("failed to load background image");
  }
  backgroundDescriptor_ = ImGui_ImplVulkan_AddTexture(backgroundTexture_->getSampler(),
                                                      backgroundTexture_->textureImageView,
                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}