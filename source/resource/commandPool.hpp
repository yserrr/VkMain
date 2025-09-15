#include "common.hpp"
#ifndef COMMANDPOOL_HPP
#define COMMANDPOOL_HPP

struct commandPoolCreateInfo{ 
    VkDevice device; 
    uint32_t queueFamilyIndex;
};

class CommandPool {
public:
CommandPool(commandPoolCreateInfo info)
:device(info.device), 
 commandPool(VK_NULL_HANDLE)
{
VkCommandPoolCreateInfo poolInfo{};
poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
poolInfo.queueFamilyIndex = info.queueFamilyIndex;
poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // 필요에 따라 조절 가능
if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
}
}
~CommandPool() {
if (commandPool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(device, commandPool, nullptr);
}
}
VkCommandPool get() const { return commandPool; }

private:
VkDevice device;
VkCommandPool commandPool;
};

#endif 
