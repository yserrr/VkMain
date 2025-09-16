#ifndef MYPROJECT_IMAGE_H
#define MYPROJECT_IMAGE_H
#include <streaming_buffer.hpp>
#include <texture_cfg.hpp>
#include <ktx.h>
#include <ktxvulkan.h>
#include <memory_allocator.hpp>

constexpr VkDeviceSize BASE_IMG_SIZE = 1024 * 1024 * 256;
constexpr uint32_t VK_TEXTURE_MAX    = 1024;

struct ImageCreateInfo{
  ImageCreateInfo(MemoryAllocator &allocator) : allocator(allocator) {}
  VkDevice device      = VK_NULL_HANDLE;
  uint32_t maxTextures = TEXTURE_MAX;
  VkSampler sampler    = VK_NULL_HANDLE;
  VkDeviceSize maxMem  = BASE_IMG_SIZE;
  MemoryAllocator &allocator;
};

enum class ImagePass: uint32_t{
  Undefined,
  Transfer,
  ShaderRead,
  ShaderWrite
};

struct VulkanTexture{
  VkImage img                 = VK_NULL_HANDLE;
  VkImageView view            = VK_NULL_HANDLE;
  VkSampler sampler           = VK_NULL_HANDLE;
  VkDescriptorSet descriptor  = VK_NULL_HANDLE;
  VkFormat imgFormat          = VK_FORMAT_R8G8B8A8_SRGB;
  VkImageViewType imgViewType = VK_IMAGE_VIEW_TYPE_2D;
  VkImageLayout imgLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
  Allocation allocation{};
  VkDeviceSize imageOffset = 0;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t mipLevels;
  uint32_t arrayLevels;
  uint32_t descriptorArrayIndex = 0;
  uint32_t bindigIndex;
  ImagePass current = ImagePass::Undefined;
  ImagePass need    = ImagePass::Transfer;
  bool upload       = false;
};

class ImageManager{
public:
  ImageManager(const ImageCreateInfo &info);
  ~ImageManager();
  void createImage(gpu::image image);
  void createImages(std::vector<gpu::image> images);
  void setStagingPool(std::shared_ptr<streamingBuffer> stagingPool);
  void setSampler(VkSampler sampler);
  void writeBufferCopyRegions(VkBufferImageCopy &region, uint32_t index);
  void writeImageBarriers(VkImageMemoryBarrier &barrier, uint32_t index);
  void setProperties(VkPhysicalDeviceProperties properties);

public:
  std::vector<std::shared_ptr<VulkanTexture> > &getTextexrures();

  StreamingBlock getBlock();
  VkDeviceSize alignUp(VkDeviceSize size) const;

private:
  MemoryAllocator &allocator_;
  VkDevice device_              = VK_NULL_HANDLE;
  VkSampler sampler_            = VK_NULL_HANDLE;
  VkDeviceSize lastOffsetCache_ = 0;
  VkDeviceSize baseMem_         = BASE_IMG_SIZE;
  VkPhysicalDeviceProperties properties_;
  VkDeviceSize copyAlign_;
  std::vector<std::shared_ptr<VulkanTexture> > textures_;
  std::shared_ptr<streamingBuffer> stagingPool_;
  StreamingBlock stagingBlock_;
  uint32_t capacity_ = 0;
  bool ownSampler_   = true;
};

#endif //MYPROJECT_IMAGE_H