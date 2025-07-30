#include "common.hpp"
#ifndef LOGICALDEVICE_HPP
#define LOGICALDEVICE_HPP

class LogicalDevice {
public:
LogicalDevice(VkPhysicalDevice physicalDevice, 
              VkSurfaceKHR    surface): 
physicalDevice(physicalDevice), 
surface(surface) 
{
    findQueueFamilies(physicalDevice, surface);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = 
    { 
        graphicsFamily.value(), 
        presentFamily.value() 
    };
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        //queue 생성
        VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
    }
    VkPhysicalDeviceFeatures deviceFeatures{};  // 필요 시 채우기
    VkDeviceCreateInfo createInfo{};
        createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos    = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures     = &deviceFeatures;
    // Device extension (예: swapchain)
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    vkGetDeviceQueue(device, graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentFamily.value(),  0, &presentQueue);
    spdlog::info("create abstract device ");
}

~LogicalDevice() {
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);}
    spdlog::info("terminate device");
}

VkDevice get()                        const { return device; }
VkPhysicalDevice getPhysicalDevice()  const { return physicalDevice; }
VkSurfaceKHR     getSurface()         const { return surface;}
VkQueue          getGraphicsQueue()   const { return graphicsQueue; }
VkQueue          getPresentQueue()    const { return presentQueue; }
uint32_t         getGraphicsFamily()  const { return graphicsFamily.value(); }
uint32_t         getPresentFamily()   const { return presentFamily.value(); }

private:
VkDevice device;
VkPhysicalDevice physicalDevice;
VkSurfaceKHR     surface;
VkQueue  graphicsQueue;
VkQueue  presentQueue;
std::optional<uint32_t> graphicsFamily;
std::optional<uint32_t> presentFamily;
    
void findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> properties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());
    for (uint32_t i = 0; i < count; i++) {
        if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            presentFamily = i;
        }
        if (graphicsFamily && presentFamily)
            return;
    }
    throw std::runtime_error("failed to find suitable queue families!");
}
};
#endif //LOGICALDEVICE_HPP