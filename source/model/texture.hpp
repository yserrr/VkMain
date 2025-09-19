#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <common.hpp>
#include <../resource/sampler_builder.hpp>
#include <stb_image.h>
#include <static_buffer.hpp>
#include <ktx/include/ktx.h>
#include <ktx/include/ktxvulkan.h>

struct TextureCreateInfo{
  VkDevice device;
  VkSampler sampler;
  const char *filename;
  MemoryAllocator *allocator;
};

//struct VulkanTexture{

//  VkImage img                 = VK_NULL_HANDLE;

//  VkImageView view            = VK_NULL_HANDLE;

//  VkSampler sampler           = VK_NULL_HANDLE;

//  VkDescriptorSet descriptor  = VK_NULL_HANDLE;

//  VkFormat imgFormat          = VK_FORMAT_R8G8B8A8_SRGB;

//  VkImageViewType imgViewType = VK_IMAGE_VIEW_TYPE_2D;

//  VkImageLayout imgLayout     = VK_IMAGE_LAYOUT_UNDEFINED;

//  Allocation allocation{};

//  VkDeviceSize imageOffset = 0;

//  uint32_t width;

//  uint32_t height;

//  uint32_t depth;

//  uint32_t mipLevels;

//  uint32_t arrayLevels;

//  uint32_t descriptorArrayIndex = 0;

//  uint32_t bindigIndex;

//  ImagePass current = ImagePass::Undefined;

//  ImagePass need    = ImagePass::Transfer;

//  bool upload       = false;

//};

struct VulkanTexture{
  VulkanTexture(TextureCreateInfo info);
  ~VulkanTexture();

  void loadImage(const char *filename);
  void uploadDescriptor(VkDescriptorSet set, uint32_t arrayIndex);
  void copyBufferToImage(VkCommandBuffer command);

  VkImageView getImageView() const
  {
    return textureImageView;
  }

  VkSampler getSampler() const
  {
    return textureSampler;
  }

  VkDevice device;
  MemoryAllocator &allocator;
  VkImage textureImage;
  VkImageView textureImageView;
  Allocation textureMemory;
  VkSampler textureSampler;
  std::unique_ptr<StaticBuffer> buffer;
  uint32_t width;
  uint32_t height;
  void createImage();
  void createImageView();
};
#endif //