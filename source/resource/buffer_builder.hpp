//
// Created by ljh on 25. 9. 16..
//

#ifndef MYPROJECT_BUFFER_BUILDER_HPP
#define MYPROJECT_BUFFER_BUILDER_HPP

#include <vulkan/vulkan.h>
#include <memory_allocator.hpp>
#include <memory_pool.hpp>
#include <unordered_map>

class streamingBuffer;

struct BufferContext{
  VkBuffer        buffer = VK_NULL_HANDLE;
  Allocation      allocation{};
  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
  uint32_t        bindingIndex  = 0;
  void *          mapped        = nullptr;
};

enum class BufferType{
  VERTEX,
  INDEX,
  UNIFORM,
  STORAGE
};

enum class AccessPolicy{
  Auto,
  HostPreferred,
  DeviceLocal
};

class BufferBuilder{
public:
  BufferBuilder(
      MemoryAllocator &allocator,
      BufferType       type,
      AccessPolicy     policy = AccessPolicy::Auto
    );
  ~BufferBuilder();
  void       buildBuffer(BufferContext &context, VkDeviceSize size, const char *debugName = "Buffer");
  void       destroy();
  void       upload(const void *data, VkDeviceSize size, VkDeviceSize dstOffset = 0);
  BufferType type() const;
  void       map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

private:
  VkBufferUsageFlags usageFromType(BufferType t) const;
  void               flush(VkDeviceSize offset, VkDeviceSize size);
  void               unmap();
  void               decidePolicy();

private:
  VkDevice         device_;
  MemoryAllocator &allocator_;
  BufferType       type_;
  AccessPolicy     policy_;

  std::unordered_map<VkBuffer, Allocation> allocations;
};

using UBOBuilder          = BufferBuilder;
using SSBOBuilder         = BufferBuilder;
using VertexBufferBuilder = BufferBuilder;
using IndexBufferBuilder  = BufferBuilder;

#endif //MYPROJECT_BUFFER_BUILDER_HPP