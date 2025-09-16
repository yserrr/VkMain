//
// Created by ljh on 25. 9. 16..
//

#include "buffer_builder.hpp"
#include "common.hpp"
#include <assert.h>

BufferBuilder::BufferBuilder(
    MemoryAllocator &allocator,
    BufferType       type,
    AccessPolicy     policy
  )
  : device_(allocator.getDevice()),
    allocator_(allocator),
    type_(type),
    policy_(policy) {}

BufferBuilder::~BufferBuilder()
{
  for (auto &allocated: allocations)
  {
    allocator_.destroyBuffer(allocated.first, allocated.second);
  }
}

void BufferBuilder::buildBuffer(BufferContext &context, VkDeviceSize size, const char *debugName)
{
  decidePolicy();
  VkBufferCreateInfo ci{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
  ci.size        = size;
  ci.usage       = usageFromType(type_);
  ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkMemoryPropertyFlags desired = (policy_ == AccessPolicy::HostPreferred) ?
                                    (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) :
                                    (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VkBuffer   newBuffer = VK_NULL_HANDLE;
  Allocation newAllocation{};
  VK_ASSERT(allocator_.createBuffer(&ci, desired, &newBuffer, &newAllocation, debugName));
  context.buffer     = newBuffer;
  context.allocation = newAllocation;
  if (desired | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
  {
    context.mapped = newAllocation.maped;
  }
  allocations[newBuffer] = newAllocation;
}

void BufferBuilder::decidePolicy()
{
  if (policy_ == AccessPolicy::Auto)
  {
    policy_ = (type_ == BufferType::UNIFORM) ? AccessPolicy::HostPreferred : AccessPolicy::DeviceLocal;
  }
}

VkBufferUsageFlags BufferBuilder::usageFromType(BufferType t) const
{
  switch (t)
  {
    case BufferType::VERTEX: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferType::INDEX: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferType::STORAGE: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    case BufferType::UNIFORM: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    default: return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  }
}

BufferType BufferBuilder::type() const
{
  return type_;
}
