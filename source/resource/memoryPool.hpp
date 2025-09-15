#include<vulkan/vulkan.h>
#include <algorithm>
#include <vector>
#include <map>
#include <stdexcept>
#ifndef MEMORYPOOL_HPP
#define MEMORYPOOL_HPP

struct Allocation{
  VkDeviceMemory memory;
  VkDeviceSize offset;
};

class MemoryPool{
public:
  MemoryPool(VkDevice device,
             uint32_t memoryTypeIndex,
             VkDeviceSize size)
    : device(device),
      memoryTypeIndex(memoryTypeIndex),
      totalSize(size)
  {
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to allocate memory pool");
    }
    freeBlocks.push_back({0, size});
    spdlog::info("create memory Pool");
  }

  ~MemoryPool()
  {
    vkFreeMemory(device, memory, nullptr);\
    spdlog::info("terminate memory pool");
  }

  bool allocate(VkDeviceSize size,
                VkDeviceSize alignment,
                Allocation &out)
  {
    for (size_t i = 0; i < freeBlocks.size(); ++i)
    {
      VkDeviceSize alignedOffset = (freeBlocks[i].offset + alignment - 1) & ~(alignment - 1);
      VkDeviceSize padding       = alignedOffset - freeBlocks[i].offset;
      if (freeBlocks[i].size >= size + padding)
      {
        out.memory             = memory;
        out.offset             = alignedOffset;
        VkDeviceSize newOffset = alignedOffset + size;
        VkDeviceSize remaining = (freeBlocks[i].offset + freeBlocks[i].size) - newOffset;
        if (remaining > 0)
        {
          freeBlocks[i] = {newOffset, remaining};
        } else
        {
          freeBlocks.erase(freeBlocks.begin() + i);
        }
        return true;
      }
    }
    return false;
  }

  void free(VkDeviceSize offset, VkDeviceSize size)
  {
    freeBlocks.push_back({offset, size});
    mergeFreeBlocks();
  }

  uint32_t getMemoryTypeIndex() const
  {
    return memoryTypeIndex;
  }

  VkDeviceMemory getMemory() const
  {
    return memory;
  }

private:
  struct FreeBlock{
    VkDeviceSize offset;
    VkDeviceSize size;
  };

  VkDevice device;
  VkDeviceMemory memory;
  uint32_t memoryTypeIndex;
  VkDeviceSize totalSize;
  std::vector<FreeBlock> freeBlocks;

  void mergeFreeBlocks()
  {
    std::sort(freeBlocks.begin(),
              freeBlocks.end(),
              [](const auto &a, const auto &b)
              {
                return a.offset < b.offset;
              });
    for (size_t i = 0; i + 1 < freeBlocks.size();)
    {
      if (freeBlocks[i].offset + freeBlocks[i].size == freeBlocks[i + 1].offset)
      {
        freeBlocks[i].size += freeBlocks[i + 1].size;
        freeBlocks.erase(freeBlocks.begin() + i + 1);
      } else
      {
        ++i;
      }
    }
  }
};

#endif //MEMORYPOOL_HPP