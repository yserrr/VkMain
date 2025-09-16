#ifndef BUFFER_HPP
#define BUFFER_HPP
#include <common.hpp>

enum class BufferType{
  Vertex,
  Index,
  Uniform,
  Storage,
  Stage
};

class Buffer{
public:
  Buffer(MemoryAllocator &allocator,
         VkDeviceSize     bufferSize,
         BufferType       type);

  ~Buffer();
  void copyBuffer(VkCommandBuffer commandBuffer);
  void createMainBuffer();
  void createUniformBuffer();
  void loadData(const void *data, VkDeviceSize size);
  void getStagingBuffer(const void *data);
  const VkBuffer *getBuffer();
  const VkBuffer getStagingBuffer();

private:
  VkDevice         device;
  VkPhysicalDevice physicalDevice;
  MemoryAllocator &allocator;
  VkBuffer         stagingBuffer;
  VkBuffer         buffer;
  Allocation       stagingAllocation;
  Allocation       allocation;
  VkDeviceSize     bufferSize;
  BufferType       type;

  bool stage(BufferType type);

  VkBufferUsageFlags getUsageFlags(BufferType type);
};
#endif //BUFFER_HPP