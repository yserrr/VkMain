#include "common.hpp"
#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP 

struct SwapchainCreateInfo{
    VkDevice             device; 
    VkPhysicalDevice     physicalDevice; 
    VkSurfaceKHR         surface; 
    uint32_t             graphicsFamily; 
    uint32_t             presentFamily;
    VkExtent2D           windowExtent;
    MemoryAllocator*     allocator;
};

class Swapchain {
public:
Swapchain(const SwapchainCreateInfo& info);
~Swapchain();
uint32_t getImageIndex(VkSemaphore semaphore);


      VkSwapchainKHR               get()          const { return    swapchain;   }
const VkSwapchainKHR*              getptr()       const { return  (&swapchain);  }
      VkFormat                     getFormat()    const { return    imageFormat; }
      VkExtent2D                   getExtent()    const { return    windowExtent;}
const std::vector<VkImage>&        getImages()    const { return    images;      }
private:

uint32_t              imageIndex;
VkDevice              device; 
VkExtent2D            windowExtent;
MemoryAllocator&      allocator;
VkSwapchainKHR        swapchain;
std::vector<VkImage>  images;
VkFormat              imageFormat;


VkSurfaceFormatKHR chooseSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface); 
VkPresentModeKHR   choosePresentMode  (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) ;
VkExtent2D         chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D windowExtent) ;
};
#endif 

