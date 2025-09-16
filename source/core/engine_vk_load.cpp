//
// Created by ljh on 25. 9. 16..
//
#include <engine.hpp>
#include <engine_vk_context.hpp>
#include <set>

const std::vector<const char *> validationLayers = {
"VK_LAYER_KHRONOS_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT             messageType,
  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
  void *                                      pUserData)
{
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
  {
    spdlog::error("Validation Layer ERROR: {}", pCallbackData->pMessage);
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
  {
    spdlog::warn("Validation Layer WARNING: {}", pCallbackData->pMessage);
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
  {
    spdlog::info("Validation Layer INFO: {}", pCallbackData->pMessage);
  } else
  {
    spdlog::debug("Validation Layer DEBUG: {}", pCallbackData->pMessage);
  }
  return VK_FALSE;
}

PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT_ptr  = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_ptr = nullptr;
PFN_vkSetDebugUtilsObjectNameEXT    g_pfnSetDebugUtilsObjectNameEXT     = nullptr;
PFN_vkSetDebugUtilsObjectTagEXT     g_pfnSetDebugUtilsObjectTagEXT      = nullptr;

inline void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
  createInfo                 = {};
  createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData       = nullptr;
}

/// only load functions

void Engine::initVulkan()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); //need to change
  window_h = glfwCreateWindow(extent.width, extent.height, title, nullptr, nullptr);

  InstanceCreateInfo instance_info{};
  bool               enableValidation = instance_info.enableValidation;

  VkApplicationInfo appInfo{};
  appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName   = instance_info.applicationName.c_str();
  appInfo.applicationVersion = instance_info.applicationVersion;
  appInfo.pEngineName        = instance_info.engineName.c_str();
  appInfo.engineVersion      = instance_info.engineVersion;
  appInfo.apiVersion         = instance_info.apiVersion;

  std::vector<const char *> enableExtensions;
  uint32_t                  glfwExtensionCount = 0;

  const char **             glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  if (enableValidation)
  {
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  spdlog::debug("Required Extensions:");
  for (const char *ext: requiredExtensions)
  {
    spdlog::debug("  {}", ext);
  }

  uint32_t instanceExtensionCount;
  vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(instanceExtensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableExtensions.data());
  spdlog::debug("Available Instance Extensions:");

  for (const auto &extension: availableExtensions)
  {
    spdlog::debug("  {}", extension.extensionName);
  }

  for (const char *requiredExtension: requiredExtensions)
  {
    bool found = false;
    for (const auto &availableExtension: availableExtensions)
    {
      if (strcmp(requiredExtension, availableExtension.extensionName) == 0)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      spdlog::error("Required instance extension '{}' not found.", requiredExtension);
      throw std::runtime_error("Required requiredExtensions not supported!");
    }
  }
  enableExtensions = requiredExtensions;

  VkDebugUtilsMessengerEXT debugMessenger;
  VkInstanceCreateInfo     vkCreateInfo{};
  vkCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkCreateInfo.pApplicationInfo        = &appInfo;
  vkCreateInfo.ppEnabledExtensionNames = enableExtensions.data();
  vkCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(enableExtensions.size());


  if (enableValidation)
  { //validation layer check
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    spdlog::debug("Available Validation Layers:");
    for (const auto &layer: availableLayers)
    {
      spdlog::debug("  {}", layer.layerName);
    }

    for (const char *layerName: validationLayers)
    {
      bool layerFound = false;
      for (const auto &layerProperties: availableLayers)
      {
        if (strcmp(layerName, layerProperties.layerName) == 0)
        {
          layerFound = true;
          break;
        }
      }
      if (!layerFound)
      {
        spdlog::error("Validation layer '{}' requested but not found.", layerName);
        throw std::runtime_error("Validation layer not supported!");
      }
    }
    std::vector<const char *> enabledLayers = validationLayers;
    vkCreateInfo.enabledLayerCount          = static_cast<uint32_t>(enabledLayers.size());
    vkCreateInfo.ppEnabledLayerNames        = enabledLayers.data();
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfoLocal{};
    populateDebugMessengerCreateInfo(debugMessengerCreateInfoLocal);
    vkCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugMessengerCreateInfoLocal;
    VkResult result = vkCreateInstance(&vkCreateInfo, nullptr, &instance_h);
    if (result != VK_SUCCESS)
    {
      spdlog::error("Failed to create VkInstance! Result: {}", static_cast<int>(result));
      throw std::runtime_error("Failed to create Vulkan instance!");
    }

  } else
  {
    vkCreateInfo.enabledLayerCount = 0;
    vkCreateInfo.pNext             = nullptr;
    VkResult result = vkCreateInstance(&vkCreateInfo, nullptr, &instance_h);
    if (result != VK_SUCCESS)
    {
      spdlog::error("Failed to create VkInstance! Result: {}", static_cast<int>(result));
      throw std::runtime_error("Failed to create Vulkan instance!");
    }

  }

  spdlog::info("Vulkan Instance created successfully.");
  if (enableValidation)
  {
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
    populateDebugMessengerCreateInfo(debugMessengerCreateInfo);
    vkCreateDebugUtilsMessengerEXT_ptr =
    reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance_h,
                                                                               "vkCreateDebugUtilsMessengerEXT"));
    vkDestroyDebugUtilsMessengerEXT_ptr
    = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance_h,
                                                                                  "vkDestroyDebugUtilsMessengerEXT"));
    g_pfnSetDebugUtilsObjectNameEXT
    = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetInstanceProcAddr(instance_h, "vkSetDebugUtilsObjectNameEXT");
    g_pfnSetDebugUtilsObjectTagEXT
    = (PFN_vkSetDebugUtilsObjectTagEXT) vkGetInstanceProcAddr(instance_h, "vkSetDebugUtilsObjectTagEXT");

    if (vkCreateDebugUtilsMessengerEXT_ptr == nullptr)
    {
      spdlog::warn("Failed to load vkCreateDebugUtilsMessengerEXT function pointer.");
      throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT!");
    }

    VkResult debugSetupResult = vkCreateDebugUtilsMessengerEXT_ptr(instance_h,
                                                                   &debugMessengerCreateInfo,
                                                                   nullptr,
                                                                   &debugMessenger);
    if (debugSetupResult != VK_SUCCESS)
    {
      spdlog::error("Failed to set up debug messenger! Result: {}", static_cast<int>(debugSetupResult));
      throw std::runtime_error("Failed to set up debug messenger!");
    }
    spdlog::info("Debug messenger setup successfully.");
  }
  VK_ASSERT(glfwCreateWindowSurface(instance_h, window_h, nullptr, &surface_h));




  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance_h, &deviceCount, nullptr);

  if (deviceCount == 0)
  {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> vk_physical_devices(deviceCount);
  vkEnumeratePhysicalDevices(instance_h, &deviceCount, vk_physical_devices.data());

  int score = 0;
  for (const VkPhysicalDevice &physical_device: vk_physical_devices)
  {
    uint32_t physicalDeviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &physicalDeviceExtensionCount, nullptr);

    std::vector<VkExtensionProperties> deviceAvailableExtensions(physicalDeviceExtensionCount);
    vkEnumerateDeviceExtensionProperties(physical_device,
                                         nullptr,
                                         &physicalDeviceExtensionCount,
                                         deviceAvailableExtensions.data());

    std::set<std::string> engineRequiredExtension = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    };

    for (const auto &extension: availableExtensions)
    {
      engineRequiredExtension.erase(extension.extensionName);
    }

    if (engineRequiredExtension.empty())
    {
      continue;
    }

    QueueFamilyIndices indices;
    uint32_t           queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount, queueFamilies.data());
    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
      const auto &queueFamily = queueFamilies[i];

      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      {
        indices.graphicsFamily = i;
      }

      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface_h, &presentSupport);
      if (presentSupport)
      {
        indices.presentFamily = i;
      }
      if (indices.isComplete())
      {
        break;
      }
    }

    if (!indices.isComplete())
    {
      throw std::runtime_error("Queue family fail to find");
    }

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physical_device, &props);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physical_device, &features);
    uint32_t tempScore = 0;

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      tempScore += 1000;
    }
    tempScore += props.limits.maxImageDimension2D;

    if (tempScore > score)
    {
      physical_device_h = physical_device;
      score             = tempScore;
      graphics_family   = indices.graphicsFamily.value();
      present_family    = indices.presentFamily.value();
    }
  }

  if (physical_device_h == VK_NULL_HANDLE)
  {
    throw std::runtime_error("failed to find a suitable GPU!");
  }

  if (enableValidation)
  {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device_h, &properties);

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

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physical_device_h, &memProperties);
    VkDeviceSize totalSize = 0;
    for (uint32_t i = 0; i < memProperties.memoryHeapCount; ++i)
    {
      if (memProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        totalSize += memProperties.memoryHeaps[i].size;
    }
    spdlog::info("Total GPU memory size: {} MB", (totalSize / (1024.0 * 1024.0)));
  }

  spdlog::info("LogicalDevice: init");

  std::set<uint32_t> uniqueQueueFamilies = {
  graphics_family,
  present_family
  };

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  float                                queuePriority = 1.0f;

  for (uint32_t queueFamily: uniqueQueueFamilies)
  {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount       = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  const std::vector<const char *> requiredDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  uint32_t                        deviceExtensionCount;
  vkEnumerateDeviceExtensionProperties(physical_device_h, nullptr, &deviceExtensionCount, nullptr);
  std::vector<VkExtensionProperties> deviceAvailableExtensions(deviceExtensionCount);
  vkEnumerateDeviceExtensionProperties(physical_device_h,
                                       nullptr,
                                       &deviceExtensionCount,
                                       deviceAvailableExtensions.data());

  std::vector<const char *> enabledExtensions;
  for (const char *extensionName: requiredDeviceExtensions)
  {
    bool found = false;
    for (const auto &ext: deviceAvailableExtensions)
    {
      if (std::strcmp(extensionName, ext.extensionName) == 0)
      {
        found = true;
        enabledExtensions.push_back(extensionName);
        break;
      }
    }
    if (!found)
    {
      throw std::runtime_error("LogicalDevice: Required device extension not supported: " + std::string(extensionName));
    }
  }
  const char *descriptorIndexingExtName = VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME;
  const char *push_desc                 = "VK_KHR_push_descriptor";

  bool descriptorIndexingExtFound = false;
  for (const auto &ext: availableExtensions)
  {
    if (std::strcmp(descriptorIndexingExtName, ext.extensionName) == 0)
    {
      descriptorIndexingExtFound = true;
      break;
    }
  }
  if (descriptorIndexingExtFound)
  {
    enabledExtensions.push_back(descriptorIndexingExtName);
    enabledExtensions.push_back(push_desc);
    spdlog::info("LogicalDevice: {} extension found and enabled.", descriptorIndexingExtName);
  } else
  {
    spdlog::warn("LogicalDevice: {} extension not found. Bindless features may not be available.",
                 descriptorIndexingExtName);
  }

  VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {};
  dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
  dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

  VkPhysicalDeviceExtendedDynamicState2FeaturesEXT dynamicPipeline2Features = {};
  dynamicPipeline2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;
  dynamicPipeline2Features.extendedDynamicState2 = VK_TRUE;

  VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{};
  indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  indexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
  indexingFeatures.runtimeDescriptorArray = VK_TRUE;
  indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
  indexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
  indexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
  indexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
  indexingFeatures.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
  indexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
  indexingFeatures.descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
  indexingFeatures.descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE;

  VkPhysicalDeviceFeatures2 deviceFeatures2{};
  deviceFeatures2.sType          = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  deviceFeatures2.pNext          = &indexingFeatures;
  indexingFeatures.pNext         = &dynamicRenderingFeatures;
  dynamicRenderingFeatures.pNext = &dynamicPipeline2Features;
  dynamicPipeline2Features.pNext = nullptr;

  vkGetPhysicalDeviceFeatures2(physical_device_h, &deviceFeatures2);

  VkDeviceCreateInfo createInfo{};
  createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos       = queueCreateInfos.data();
  createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.enabledExtensionCount   = static_cast<uint32_t>(enabledExtensions.size());
  createInfo.ppEnabledExtensionNames = enabledExtensions.data();
  createInfo.pEnabledFeatures        = nullptr;
  createInfo.pNext                   = &deviceFeatures2;

  VK_ASSERT(vkCreateDevice(physical_device_h, &createInfo, nullptr, &device_h));
  spdlog::info("LogicalDevice: Device created successfully.");

  vkGetDeviceQueue(device_h, graphics_family, 0, &graphics_q);
  vkGetDeviceQueue(device_h, present_family, 0, &present_q);
  spdlog::info("LogicalDevice: Queues obtained.");

  allocator   = std::make_unique<MemoryAllocator>(physical_device_h, device_h);
  interaction = std::make_unique<EventManager>(window_h);
}