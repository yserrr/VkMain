#include <texture.hpp>

Texture::Texture(textureCreateInfo info)
  : device(info.device),
    textureSampler(info.sampler),
    allocator(*info.allocator)
{
  loadImage(info.filename);
}

Texture::~Texture()
{
  vkDestroyImageView(device, textureImageView, nullptr);
  vkDestroyImage(device, textureImage, nullptr);
}

void Texture::loadImage(const char *filename)
{
  int32_t texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
  if (!pixels) throw std::runtime_error("failed to load texture image!");
  VkDeviceSize imageSize = texWidth * texHeight * 4;
  width                  = texWidth;
  height                 = texHeight;
  createImage();
  createImageView();
  buffer = std::make_unique<Buffer>(allocator, imageSize, BufferType::Stage);
  buffer->getStagingBuffer(pixels);
  spdlog::info("create staging buffer for texture");
  stbi_image_free(pixels);
}

//자기 정보를 descriptor hander를 받아서 update
void Texture::uploadDescriptor(VkDescriptorSet set)
{
  if (textureSampler == VK_NULL_HANDLE)
  {
    spdlog::info("fuck");
  }
  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView   = textureImageView; // VkImageView
  imageInfo.sampler     = textureSampler;   // VkSampler

  VkWriteDescriptorSet descriptorWrite{};
  descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet          = set;
  descriptorWrite.dstBinding      = 1; // bindingIndex; // shader에서의 binding 번호
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pImageInfo      = &imageInfo;
  vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

//upload 방법을 통해서 호출
void Texture::createImage()
{
  VkImageCreateInfo imageInfo{};
  imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType     = VK_IMAGE_TYPE_2D;
  imageInfo.flags         = 0;
  imageInfo.extent.width  = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth  = 1;
  imageInfo.mipLevels     = 1;
  imageInfo.arrayLayers   = 1;
  imageInfo.format        = VK_FORMAT_R8G8B8A8_UNORM;
  imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL; //gpu optional
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.pNext         = nullptr;
  if (vkCreateImage(device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
  {
    std::runtime_error("fail to make texture Image buffer");
  }
  VkMemoryRequirements memoryReq;
  vkGetImageMemoryRequirements(device, textureImage, &memoryReq);
  textureMemory = allocator.allocate(memoryReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkBindImageMemory(device, textureImage, textureMemory.memory, textureMemory.offset);
}

void Texture::createImageView()
{
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image                           = textureImage;
  viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format                          = VK_FORMAT_R8G8B8A8_UNORM;
  viewInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel   = 0;
  viewInfo.subresourceRange.levelCount     = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount     = 1;
  if (vkCreateImageView(device, &viewInfo, nullptr, &textureImageView) != VK_SUCCESS)
  {
    throw std::runtime_error("error to make texture view");
  }
}

void Texture::copyBufferToImage(VkCommandBuffer command)
{
// 1. stagingBuffer와 deviceLocalImage(VkImage) 생성 (생략)
// 2. 커맨드 버퍼 기록
// 3. 이미지 레이아웃 전환 (transfer dst로)
  VkImageMemoryBarrier barrier{};
  barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
  barrier.image                           = textureImage;
  barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel   = 0;
  barrier.subresourceRange.levelCount     = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount     = 1;
  barrier.srcAccessMask                   = 0;
  barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
  vkCmdPipelineBarrier(
      command,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0,
      0,
      nullptr,
      0,
      nullptr,
      1,
      &barrier
    );
// 4. 버퍼에서 이미지로 복사
  VkBufferImageCopy region{};
  region.bufferOffset                    = 0;
  region.bufferRowLength                 = 0;
  region.bufferImageHeight               = 0;
  region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel       = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount     = 1;
  region.imageOffset                     = {0, 0, 0};
  region.imageExtent                     = {width, height, 1};
  spdlog::info("call to command buffer to trenslate to texture");
  vkCmdCopyBufferToImage(
      command,
      buffer->getStagingBuffer(),
      textureImage,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1,
      &region
    );
// 5. 이미지 레이아웃을 셰이더 읽기용으로 전환
  barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  vkCmdPipelineBarrier(
      command,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0,
      0,
      nullptr,
      0,
      nullptr,
      1,
      &barrier
    );
}