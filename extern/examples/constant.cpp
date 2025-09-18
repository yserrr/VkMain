//
// Created by ljh on 25. 8. 24..
//

#include "constant.hpp"

Constant::Constant()
{




}

void flush(VkDeviceSize offset, VkDeviceSize size)
{
  if (coherent_) return;
  VkMappedMemoryRange range{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
  range.memory = allocation_.memory;
  range.offset = allocation_.offset;
  range.size   = VK_WHOLE_SIZE;
  vkFlushMappedMemoryRanges(device_, 1, &range);
}