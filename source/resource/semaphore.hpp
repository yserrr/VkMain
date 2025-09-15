#include <common.hpp> 
#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP 
//fence ,semaphore 둘다 사용
//flag 등의 fence 추가 사용의 경우, 추가적으로 생성자 오버로딩하도록 설장
struct signalCreateInfo{ 
    VkDevice device; 
    uint32_t MAX_FRAMES_IN_FLIGHT;
};

class SemaphoreManager {
public:
SemaphoreManager(signalCreateInfo& info)  :
  device(info.device),
  MAX_FRAMES_IN_FLIGHT(info.MAX_FRAMES_IN_FLIGHT),
  semaphores(info.MAX_FRAMES_IN_FLIGHT)
{
VkSemaphoreCreateInfo createInfo {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    if (vkCreateSemaphore(device, &createInfo, nullptr, &semaphores[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphore for frame " + std::to_string(i));
    }
}
}
~SemaphoreManager() {
for (VkSemaphore semaphore: semaphores)
if(semaphore!= VK_NULL_HANDLE) vkDestroySemaphore(device, semaphore, nullptr);
}
VkSemaphore get(uint32_t currentFrame) const { return semaphores[currentFrame]; }
private:
    VkDevice device;
    uint32_t MAX_FRAMES_IN_FLIGHT;
    std::vector<VkSemaphore> semaphores;
};




#endif //semaphore_hpp