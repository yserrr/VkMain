#include <semaphore.hpp> 
#ifndef FENCE_HPP
#define FENCE_HPP

class FenceManager {
public:
FenceManager(signalCreateInfo info, bool signaled= false) 
: device(info.device),
  MAX_FRAMES_IN_FLIGHT(info.MAX_FRAMES_IN_FLIGHT), 
  fences(info.MAX_FRAMES_IN_FLIGHT) 
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType     = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled) fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for(VkFence& fence: fences){
        if(vkCreateFence(device, &fenceInfo, nullptr,&fence)!=VK_SUCCESS){
            throw std::runtime_error("failed to create fence");
        }
    }
    spdlog::info("create fence");
}
~FenceManager(){
for(VkFence& fence: fences)
   if(fence!= VK_NULL_HANDLE) vkDestroyFence(device, fence, nullptr);
}
VkFence get(uint32_t currentFrame) const 
    { return fences[currentFrame]; }
    
void wait(uint32_t currentFrame, uint64_t timeout = UINT64_MAX) {
vkWaitForFences(device, 1, &fences[currentFrame], VK_TRUE, timeout);
}
void reset(uint32_t currentFrame)
{vkResetFences(device, 1, &fences[currentFrame]); }

private:
VkDevice              device;
std::vector<VkFence>  fences;
uint32_t              MAX_FRAMES_IN_FLIGHT;
};

#endif 