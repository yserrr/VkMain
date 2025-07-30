#include <swapChain.hpp>

Swapchain::Swapchain(const SwapchainCreateInfo& info): 
 device      (info.device),
 swapchain   (VK_NULL_HANDLE),
 allocator   (*(info.allocator)),
 windowExtent(info.windowExtent){
//Surface capabilities 확인 
VkSurfaceCapabilitiesKHR capabilities; 
if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info.physicalDevice, info.surface, &capabilities)!=VK_SUCCESS)
throw std::runtime_error("fail to find surface capability");
// 2. Surface format 선택
VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(info.physicalDevice, info.surface);
VkPresentModeKHR   presentMode   = choosePresentMode  (info.physicalDevice, info.surface);
windowExtent       = chooseExtent(capabilities, windowExtent);
//image capability setting
uint32_t imageCount = capabilities.minImageCount + 1;
if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
}
// 3. 스왑체인 구조체
VkSwapchainCreateInfoKHR createInfo{};
createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
createInfo.surface          = info.surface;
createInfo.minImageCount    = imageCount;
createInfo.imageFormat      = surfaceFormat.format;
createInfo.imageColorSpace  = surfaceFormat.colorSpace;
createInfo.imageExtent      = windowExtent;
createInfo.imageArrayLayers = 1;
createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

//queue family 정보 받기
uint32_t queueFamilyIndices[] = { info.graphicsFamily, info.presentFamily };
if (info.graphicsFamily != info.presentFamily) {
    createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices   = queueFamilyIndices;
} else {
    createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
}
createInfo.preTransform   = capabilities.currentTransform;
createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; 
createInfo.presentMode    = presentMode;
createInfo.clipped        = VK_TRUE;
createInfo.oldSwapchain   = VK_NULL_HANDLE;
//swapchain 생성하기
if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swapchain!");
}
//  이미지 가져오기
uint32_t swapImageCount = 0;
if(vkGetSwapchainImagesKHR(info.device, swapchain, &swapImageCount, nullptr)!= VK_SUCCESS){ 
    throw std::runtime_error("fail to get image in swapchain");
}
// swapchain -> image연결
images.resize(swapImageCount);
vkGetSwapchainImagesKHR(device, swapchain, &swapImageCount, images.data());
imageFormat  = surfaceFormat.format;
windowExtent = createInfo.imageExtent;
spdlog::info("create swapchain");

}

Swapchain::~Swapchain(){ 
if (swapchain != VK_NULL_HANDLE) {vkDestroySwapchainKHR(device, swapchain, nullptr);}}

//swapchain 이 next image index를 결정하도록 구성
uint32_t Swapchain:: getImageIndex(VkSemaphore semaphore){ 
    if(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphore,  VK_NULL_HANDLE, &imageIndex)!= VK_SUCCESS)
        throw std::runtime_error("error to get image");             
    return imageIndex;
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
uint32_t count;
vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
std::vector<VkSurfaceFormatKHR> formats(count);
vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());
for (const auto& available : formats) {
    if (available.format     == VK_FORMAT_B8G8R8A8_SRGB &&
        available.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return available;
    }
}
return formats[0]; // fallback
}

VkPresentModeKHR Swapchain::choosePresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
uint32_t count;
vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
std::vector<VkPresentModeKHR> modes(count);
vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, modes.data());
for (const auto& mode : modes) {
    //비동기->MAILBOX
    if (mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
        return mode;
    }
}
return VK_PRESENT_MODE_FIFO_KHR; // 보장된 기본 모드
}

VkExtent2D  Swapchain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D windowExtent) {
if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
} else {
    VkExtent2D actualExtent = windowExtent;
    actualExtent.width  = std::max(capabilities.minImageExtent.width,  std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
    return actualExtent;
    }
}