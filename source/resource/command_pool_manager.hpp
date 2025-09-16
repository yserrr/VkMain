#ifndef COMMANDBUFFER_HPP
#define COMMANDBUFFER_HPP
#include "common.hpp"

//commandBuffer에서 command list 전체 관리 및 렌더링 작업 진행
struct CommandPoolManagerCreateInfo{
  VkDevice device;
  VkCommandPool commandPool;
  uint32_t frameCount;
};

class CommandPoolManager{
public:
  CommandPoolManager(const CommandPoolManagerCreateInfo &info)
    : device(info.device),
      commandPool(info.commandPool)
  {
    commandBuffers.resize(info.frameCount);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = info.frameCount;
    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to allocate command buffers!");
    }

  }
  ~CommandPoolManager()
  {
    if (commandPool != VK_NULL_HANDLE)
      vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
  }

  VkCommandBuffer record(uint32_t imageIndex)
  {
    if (vkResetCommandBuffer(commandBuffers[imageIndex], 0) != VK_SUCCESS)
      throw std::runtime_error("fail to reset command buffer");
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffers[imageIndex];
  }

  void endRecord(uint32_t imageIndex)
  {
    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
    {
      spdlog::error("Command buffer recording failed");
    }
  }

private:
  VkDevice device;
  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;
};

#endif