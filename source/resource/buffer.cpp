#include<buffer.hpp> 

Buffer::Buffer(MemoryAllocator &allocator, VkDeviceSize bufferSize, BufferType type): allocator(allocator),
                                                                                      bufferSize(bufferSize),
                                                                                      type(type) {
  device         = allocator.getDevice();
  physicalDevice = allocator.getPhysicalDevice();
  bool STAGE     = stage(type);
}

Buffer::~Buffer() {
  vkDeviceWaitIdle(device);
  if (stagingBuffer != VK_NULL_HANDLE) vkDestroyBuffer(device, stagingBuffer, nullptr);
  if (buffer != VK_NULL_HANDLE)vkDestroyBuffer(device, buffer, nullptr);
  allocator.free(stagingAllocation, bufferSize);
  allocator.free(allocation, bufferSize);
}
void               Buffer::copyBuffer(VkCommandBuffer commandBuffer) {
  VkDeviceSize offsets = 0;
  VkBufferCopy bufferCopy{};
  bufferCopy.srcOffset = 0;
  bufferCopy.dstOffset = 0;
  bufferCopy.size      = bufferSize;
  vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer, 1, &bufferCopy);
}
void               Buffer::createMainBuffer() {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size        = bufferSize;
  bufferInfo.usage       = getUsageFlags(type);
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
  VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  allocation                       = allocator.allocate(memRequirements, properties);

  if (vkBindBufferMemory(device, buffer, allocation.memory, allocation.offset) != VK_SUCCESS)
  {
    spdlog::info("falil to bind buffer to memory");
  }
}
void               Buffer::createUniformBuffer() {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size        = bufferSize;
  bufferInfo.usage       = getUsageFlags(type);
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
  VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  allocation = allocator.allocate(memRequirements, properties);

  if (vkBindBufferMemory(device, buffer, allocation.memory, allocation.offset) != VK_SUCCESS)
  {
    spdlog::info("falil to bind buffer to memory");
  }
}
void               Buffer::loadData(const void *data, VkDeviceSize size) {
  void *bufferData;
  vkMapMemory(device, allocation.memory, allocation.offset, size, 0, &bufferData);
  memcpy(bufferData, data, (size_t) size);
  vkUnmapMemory(device, allocation.memory);
}
void               Buffer::getStagingBuffer(const void *data) { ///

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size        = bufferSize;
  bufferInfo.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; // 복사 소스
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  // 2. 버퍼 생성
  if (vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create staging buffer!");
  }
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);
  VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  stagingAllocation = allocator.allocate(memRequirements, properties);
  vkBindBufferMemory(device, stagingBuffer, stagingAllocation.memory, stagingAllocation.offset);
  void *bufferData;
  vkMapMemory(device, stagingAllocation.memory, stagingAllocation.offset, bufferSize, 0, &bufferData);
  memcpy(bufferData, data, (size_t) bufferSize);
  vkUnmapMemory(device, stagingAllocation.memory);
}
const VkBuffer *   Buffer::getBuffer() {
  return &buffer;
}
const VkBuffer     Buffer::getStagingBuffer() {
  return stagingBuffer;
}
bool               Buffer::stage(BufferType type) {
  switch (type)
  {
    case BufferType::Vertex:
    case BufferType::Index:
    case BufferType::Stage:
      return true;
    case BufferType::Uniform:
    case BufferType::Storage:
      return false;
  }
  return false; // or assert
}
VkBufferUsageFlags Buffer::getUsageFlags(BufferType type) {
  switch (type)
  {
    case BufferType::Vertex:
      return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferType::Index:
      return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferType::Uniform:
      return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    case BufferType::Storage:
      return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  }
  return 0; // or assert
}