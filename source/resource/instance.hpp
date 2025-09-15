#include"common.hpp"
#ifndef INSTANCE_HPP 
#define INSTANCE_HPP

// 레이어 이름
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
}; 

// 디버깅 콜백 함수
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT              messageType,
    const VkDebugUtilsMessengerCallbackDataEXT*  pCallbackData,
    void*                                        pUserData) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}

// Debug Messenger 생성 함수 포인터 로드
inline VkResult CreateDebugUtilsMessengerEXT(
    VkInstance                                instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks*              pAllocator,
    VkDebugUtilsMessengerEXT*                 pDebugMessenger) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


class Instance{ 
public:
Instance() 
:instance(VK_NULL_HANDLE)
{   //info -> create info 
getRequiredExtensions();
VkApplicationInfo appInfo{};  
appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO; 
appInfo.pApplicationName    = "myEngine"; 
appInfo.applicationVersion  = VK_MAKE_VERSION(1,0,0); 
appInfo.pEngineName         = "noEngine";
appInfo.engineVersion       = VK_MAKE_VERSION(1,0,0); 
appInfo.apiVersion          = VK_API_VERSION_1_0; 
VkInstanceCreateInfo createInfo{}; 
createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
createInfo.pApplicationInfo        = &appInfo;
createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
createInfo.ppEnabledExtensionNames = extensions.data();
createInfo.enabledLayerCount       = static_cast<uint32_t>(validationLayers.size());
createInfo.ppEnabledLayerNames     = validationLayers.data();
// Debug Messenger 생성 정보
VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
populateDebugMessengerCreateInfo(debugCreateInfo);
createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create Vulkan instance");
}
if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        std::cerr << "failed to set up debug messenger!" << std::endl;
}
spdlog::info("create instance");
}
~Instance()
{ 
if (instance != VK_NULL_HANDLE) {
    vkDestroyInstance(instance, nullptr);
}
spdlog::info("terminate instance");
}

VkInstance get() const {return instance ;}
private: 
VkInstance instance ;
std::vector<const char*> enabledExtensions;  // 복사해서 보관 가능
std::vector<const char*> extensions;
VkDebugUtilsMessengerEXT debugMessenger;

void getRequiredExtensions(){ 
    // GET REQUIRED  vector 재할당
    //사용가능한 EXTENSION 확인: ex::UBUNTU X11 ...
    uint32_t    glfwExtensionCount= 0;
    const char**glfwExtensions    = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    extensions.assign(glfwExtensions, glfwExtensions+glfwExtensionCount); 
    extensions.push_back("VK_EXT_debug_utils"); // 안 넣었기 때문에 에러 발생
    for(const char* i: extensions)   
            std::cout<< i<<"\n";
    } 
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = 
                                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;
    }
};

#endif// 