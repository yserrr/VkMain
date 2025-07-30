#include<uiRenderer.hpp>

UIRenderer::UIRenderer(UIRendererCreateInfo info)
:device_h         (info.device_h), 
 instance_h       (info.instance_h),
 physical_device_h(info.physical_device_h),
 window_h         (info.window_h),
 renderpass_h     (info.renderpass_h), 
 graphics_family  (info.graphics_family), 
 present_family   (info.present_family), 
 graphics_q       (info.graphics_q)
{
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io=ImGui::GetIO(); 
ImGui::StyleColorsDark();  
createPool();
//find API LEVEL imformation
VkPhysicalDeviceProperties   properties; 
vkGetPhysicalDeviceProperties(physical_device_h, &properties);
ImGui_ImplVulkan_InitInfo UIinfo = {};
UIinfo.Instance        = instance_h;
UIinfo.PhysicalDevice  = physical_device_h;
UIinfo.Device          = device_h;
UIinfo.QueueFamily     = graphics_family;
UIinfo.Queue           = graphics_q;
UIinfo.PipelineCache   = VK_NULL_HANDLE;
UIinfo.DescriptorPool  = imguiPool;
UIinfo.Allocator       = nullptr;
UIinfo.MinImageCount   = 2;
UIinfo.ImageCount      = 3;
UIinfo.CheckVkResultFn = nullptr; // 필요하면 콜백 등록
UIinfo.RenderPass      = renderpass_h;
UIinfo.ApiVersion      = properties.apiVersion;
UIinfo.Subpass         = 1;
//info.UseDynamicRendering = true;
//imgui용 renderpass분리하기
ImGui_ImplGlfw_InitForVulkan(window_h, true);
ImGui_ImplVulkan_Init(&UIinfo);
spdlog::info("start imgui");
}

void UIRenderer::draw(VkCommandBuffer command){ 
// 1. 새 프레임 시작
ImGui_ImplVulkan_NewFrame();
ImGui_ImplGlfw_NewFrame();
ImGui::NewFrame();

// 2. UI 구성
ImGui::Begin("Hello");
ImGui::Text("ImGui Vulkan Example");
ImGui::End();
// 3. 렌더링 준비
ImGui::Render();
ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command);
}


void UIRenderer:: createPool(){ 
VkDescriptorPoolSize pool_sizes[] ={
{ VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }
    };
VkDescriptorPoolCreateInfo pool_info = {};
pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
pool_info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
pool_info.maxSets       = 1000 * IM_ARRAYSIZE(pool_sizes);
pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
pool_info.pPoolSizes    = pool_sizes;
if(vkCreateDescriptorPool(device_h, &pool_info, nullptr, &imguiPool)!=VK_SUCCESS){
    throw std::runtime_error("fail to create Imgui Pool");
    }
}