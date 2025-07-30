#include "common.hpp"
#ifndef PHYSICALDEVICE_HPP
#define PHYSICALDEVICE_HPP 
// vkEnumratePhysical device -> get list 
// rate the  physical device extension and function 
// queue family, memory
// select GPU
class PhysicalDevice{ 
public:
PhysicalDevice(VkInstance   instance, 
               VkSurfaceKHR surface )
:physicalDevice(VK_NULL_HANDLE)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) throw std::runtime_error("failed to find GPUs with Vulkan support!");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    int score = 0; 
    int temp;
    for (const VkPhysicalDevice& device : devices) {
        temp= rateDevice(device,surface); 
        if(temp>score){
            physicalDevice = device; 
            score = temp;
        } 
    }
    if (physicalDevice == VK_NULL_HANDLE) throw std::runtime_error("failed to find a suitable GPU!");
    //get properties for devices name check
    VkPhysicalDeviceProperties    properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    //info for physical device
} 
    
VkPhysicalDevice get() const { return physicalDevice; }
private:
VkPhysicalDevice physicalDevice;
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};
// 여기서 queue family, 확장, swapchain 지원 검사 구현
// 외장 GPU 가산  
// image buffer size 가산
// 필수 확장 기능 확인 후 평가
// queue family 있어어함 
// compute 쉐이더 있으면 가산
// 외장 GPU면 점수 높은 점수, 대부분의 외장 gpu가 성능이 좋음
// 많은 경우가 1개의 외장, 1개의 내장 gpu를 사용하게 됨 
// 그러나, 외장 gpu가 여러개의 경우, 아래에서 추가 점수로 구분

int rateDevice(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties   props;
    vkGetPhysicalDeviceProperties(device, &props);
    VkPhysicalDeviceFeatures    features;
    vkGetPhysicalDeviceFeatures(device, &features);
    int score = 0;
    if (!checkDeviceExtensionSupport(device)) return 0; // 필수 확장 체크 (예: swapchain)
    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    if (!indices.isComplete()) return 0;                //queue family를 지원하지 않으면 탈락

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
    score += props.limits.maxImageDimension2D; // 최대 2D 이미지 크기 추가
    return score;
}

//queue check : 필수 지원 queue 확인
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t           queueFamilyCount = 0;
    QueueFamilyIndices indices;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr); //개수확인
    std::vector<VkQueueFamilyProperties>    queueFamilies(queueFamilyCount);      // 할당 
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        const auto& queueFamily = queueFamilies[i];
        // 1. Graphics 지원
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        // 2. Presentation 지원 (GLFW + Surface)
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }
        if (indices.isComplete()) {
            break;
        }
    }
    return indices;
}
bool checkDeviceExtensionSupport(VkPhysicalDevice device){
    uint32_t extensionCount = 0; 
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties>  availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    std::set<std::string> requiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME // for imgui setting
    };
    for (std::string s: requiredExtensions){
        //spdlog::info(s);
    }
    
    for (const auto& extension : availableExtensions) {
        //spdlog::info(extension.extensionName);
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();  // 모두 제거됐다면, 모두 지원된다는 뜻
}


void info(VkPhysicalDeviceProperties properties){
    spdlog::info("Physical device name: {}", properties.deviceName);
    spdlog::info("API version: {}.{}.{}", 
                 VK_VERSION_MAJOR(properties.apiVersion),
                 VK_VERSION_MINOR(properties.apiVersion),
                 VK_VERSION_PATCH(properties.apiVersion));
    spdlog::info("Vendor ID: 0x{:04X}", properties.vendorID);
    spdlog::info("Device ID: 0x{:04X}", properties.deviceID);

    spdlog::info("Max 2D image dimension: {}", properties.limits.maxImageDimension2D);
    spdlog::info("Max 3D image dimension: {}", properties.limits.maxImageDimension3D);
    spdlog::info("Max cube image dimension: {}", properties.limits.maxImageDimensionCube);
    spdlog::info("Max sampler allocation count: {}", properties.limits.maxSamplerAllocationCount);
    spdlog::info("Max descriptor set storage images: {}", properties.limits.maxDescriptorSetStorageImages);
    spdlog::info("Max push constants size: {} bytes", properties.limits.maxPushConstantsSize);
    spdlog::info("Max vertex input attributes: {}", properties.limits.maxVertexInputAttributes);
    spdlog::info("Max framebuffer width: {}", properties.limits.maxFramebufferWidth);
    spdlog::info("Max framebuffer height: {}", properties.limits.maxFramebufferHeight);

    spdlog::info("Framebuffer depth sample counts (bitmask): 0x{:X}", properties.limits.framebufferDepthSampleCounts);
    spdlog::info("Framebuffer color sample counts (bitmask): 0x{:X}", properties.limits.framebufferColorSampleCounts);

    spdlog::info("Max compute work group invocations: {}", properties.limits.maxComputeWorkGroupInvocations);
    spdlog::info("Max compute work group size: x={}, y={}, z={}",
                 properties.limits.maxComputeWorkGroupSize[0],
                 properties.limits.maxComputeWorkGroupSize[1],
                 properties.limits.maxComputeWorkGroupSize[2]);
    spdlog::info("Max compute work group count: x={}, y={}, z={}",
                 properties.limits.maxComputeWorkGroupCount[0],
                 properties.limits.maxComputeWorkGroupCount[1],
                 properties.limits.maxComputeWorkGroupCount[2]);
    
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    VkDeviceSize totalSize = 0;
    for (uint32_t i = 0; i < memProperties.memoryHeapCount; ++i) {
        if (memProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            totalSize += memProperties.memoryHeaps[i].size;
        }
    }
    spdlog::info ("Total GPU memory size: {} MB" ,(totalSize / (1024.0 * 1024.0)));
}
};

#endif //PHYSICALDEVICE_HPP