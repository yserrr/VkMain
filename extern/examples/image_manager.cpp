#include "image_manager.hpp"

ImageManager::ImageManager(const ImageCreateInfo &info) :
  device_(info.device),
  allocator_(info.allocator),
  capacity_(info.maxTextures),
  baseMem_(info.maxMem),
  sampler_(info.sampler)
{
  textures_.resize(info.maxTextures);
}

ImageManager::~ImageManager()
{
  for (int i = 0; i < textures_.size(); ++i)
  {
    //if (textures_[i]->view != VK_NULL_HANDLE) vkDestroyImageView(device_, textures_[i]->view, nullptr);
    //if (textures_[i]->img != VK_NULL_HANDLE) allocator_.destroyImage(textures_[i]->img, textures_[i]->allocation);
  }
}

void ImageManager::createImage(gpu::image img)
{
  if (stagingBlock_.buffer == VK_NULL_HANDLE)
  {
    //stagePool was setted before texture setting
    throw std::runtime_error("need to allocate block");
  }
  std::shared_ptr<VulkanTexture> texture = std::shared_ptr<VulkanTexture>();

  uint32_t index              = img.index;
  VkImageCreateInfo imageInfo = {};
  imageInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType         = (img.dim == TexDim::Tex2D) ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
  imageInfo.extent.width      = img.width;
  imageInfo.extent.height     = img.height;
  imageInfo.extent.depth      = img.depth;
  imageInfo.mipLevels         = 1;
  imageInfo.arrayLayers       = 1;
  imageInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
  switch (img.format)
  {
    case (Format::R8G8B8A8_SRGB):
    {
      imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
      break;
    }
    case (Format::R8G8B8A8_UNORM):
    {
      imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
      break;
    }
    case (Format::R8_UNORM):
    {
      imageInfo.format = VK_FORMAT_R8_UNORM;
      break;
    }
    case (Format::D32_SFLOAT):
    {
      imageInfo.format = VK_FORMAT_D32_SFLOAT;
      break;
    }
    default:
      imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
      break;
  }
  imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  texture->depth                = img.depth;
  texture->width                = img.width;
  texture->height               = img.height;
  texture->arrayLevels          = img.arrayLevels;
  texture->mipLevels            = img.mipLevels;
  texture->imgFormat            = imageInfo.format;
  texture->descriptorArrayIndex = index;
  VK_ASSERT(allocator_.createImage(&imageInfo,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    &(texture->img),
    &(texture->allocation)));

  ///todo : view setting with cube and array
  /// aspect bit flag need;
  /// segment and change the logic

  VkImageViewCreateInfo viewCi           = {};
  viewCi.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewCi.image                           = texture->img;
  viewCi.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
  viewCi.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  viewCi.subresourceRange.baseMipLevel   = 0;
  viewCi.subresourceRange.levelCount     = 1;
  viewCi.subresourceRange.baseArrayLayer = 0;
  viewCi.subresourceRange.layerCount     = 1;
  viewCi.format                          = imageInfo.format;
  VK_ASSERT(vkCreateImageView(device_, &viewCi, nullptr, &texture->view));

  VkDeviceSize imageSize, tmp;
  imageSize = img.width * img.height * 4;
  stagingPool_->map(img.data, stagingBlock_.offset + lastOffsetCache_, imageSize);
  tmp = alignUp(imageSize);
  lastOffsetCache_ += tmp;
  texture->imageOffset = lastOffsetCache_;
  texture->imgViewType = viewCi.viewType;
  texture->imgLayout   = imageInfo.initialLayout;
  texture->sampler     = sampler_;
  texture->current     = ImagePass::Undefined;
  texture->need        = ImagePass::Transfer;
  textures_[index]     = (std::move(texture));
}

void ImageManager::createImages(std::vector<gpu::image> images)
{
  for (int i = 0; i < images.size(); i++)
  {
    createImage(images[i]);
  }
}

void ImageManager::setStagingPool(std::shared_ptr<streamingBuffer> stagingPool)
{
  stagingPool_  = stagingPool;
  stagingBlock_ = stagingPool->acquire(baseMem_, MinAlignType::IMAGE);
}

void ImageManager::setSampler(VkSampler sampler)
{
  sampler_ = sampler;
}

void ImageManager::writeBufferCopyRegions(VkBufferImageCopy &region, uint32_t index)
{
  region.bufferOffset      = textures_[index]->imageOffset;
  region.bufferRowLength   = 0;
  region.bufferImageHeight = 0;
  region.imageOffset       = {0, 0, 0};
  ///sub resource change need
  ///this time -> just 001
  region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel       = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount     = 1;
  region.imageExtent                     = {
  textures_[index]->width,
  textures_[index]->height,
  textures_[index]->depth
  };
}

void ImageManager::writeImageBarriers(VkImageMemoryBarrier &barrier, uint32_t index)
{
  switch (textures_[index]->need)
  {
    case (ImagePass::ShaderRead):
    {
      barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;     // copy 후 layout
      barrier.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // shader에서 읽을 layout
      barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;             // copy가 끝난 후
      barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;                // fragment shader에서 읽기 위해
      barrier.image                           = textures_[index]->img;                    // 해당 image handle
      barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseMipLevel   = 0;
      barrier.subresourceRange.levelCount     = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount     = 1;
      textures_[index]->need                  = ImagePass::Undefined;
      textures_[index]->upload                = true;
      return;
    }
    case (ImagePass::Transfer):
    {
      barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
      barrier.image                           = textures_[index]->img;
      barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseMipLevel   = 0;
      barrier.subresourceRange.levelCount     = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount     = 1;
      barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
      barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.srcAccessMask                   = 0;
      barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
      textures_[index]->need                  = ImagePass::ShaderRead;
      return;
    }
    default: return;
  }
}

StreamingBlock ImageManager::getBlock()
{
  return stagingBlock_;
}

std::vector<std::shared_ptr<VulkanTexture> > &ImageManager::getTextexrures()
{
  return textures_;
}

void ImageManager::setProperties(VkPhysicalDeviceProperties properties)
{
  copyAlign_  = properties.limits.optimalBufferCopyOffsetAlignment;
  properties_ = properties;
}

VkDeviceSize ImageManager::alignUp(VkDeviceSize size) const
{
  return (size + (copyAlign_ - 1)) & ~(copyAlign_ - 1);
}