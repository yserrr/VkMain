#include <memoryPool.hpp> 


#ifndef MEMORYALLOCATOR
#define MEMORYALLOCATOR 

class MemoryAllocator {
public:
MemoryAllocator(VkPhysicalDevice physicalDevice, 
                VkDevice device)
:physicalDevice(physicalDevice), 
 device(device) {
 }
~MemoryAllocator(){
    for (auto* pool : pools) {
        if(pool!= VK_NULL_HANDLE) delete pool;
    }
}

// Search for existing pool
Allocation allocate(VkMemoryRequirements  requirements, VkMemoryPropertyFlags desiredFlags){
    uint32_t memoryType = findMemoryType(requirements.memoryTypeBits,desiredFlags);
    for (auto* pool : pools) { 
        if (pool->getMemoryTypeIndex() == memoryType) {
            Allocation result;
            if (pool->allocate(requirements.size, requirements.alignment, result)) {
                return result;
            }
        }
    }
    // Create new pool 
    VkDeviceSize poolSize = std::max(requirements.size * 8, (VkDeviceSize)256 * 1024 * 1024); // 256MB 기본
    MemoryPool*  newPool  = new MemoryPool(device, memoryType, poolSize);
    pools.push_back(newPool);
    Allocation result;
    if (!newPool->allocate(requirements.size, requirements.alignment, result)) {
        throw std::runtime_error("Failed to allocate memory from new pool");
    }
    return result;
}
void free(Allocation allocation, VkDeviceSize size){
    for (auto* pool : pools) {
            if (pool->getMemory() == allocation.memory) {
                pool->free(allocation.offset, size);
                return;
            }
        }
        throw std::runtime_error("Tried to free memory from unknown pool");
}
VkDevice         getDevice()         {return device;}
VkPhysicalDevice getPhysicalDevice() {return physicalDevice;}
    
private:
std::vector<MemoryPool*> pools;
VkPhysicalDevice         physicalDevice;
VkDevice                 device;

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
VkPhysicalDeviceMemoryProperties memProperties;
vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
        return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type");
}
};



#endif //MEMORYALLOCATOR_HPP 


