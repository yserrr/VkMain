#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <common.hpp> 
#include <sampler.hpp>
#include <stb_image.h>
#include <buffer.hpp>

struct textureCreateInfo{
VkDevice               device; 
VkSampler              sampler;
const char*            filename;  
MemoryAllocator*       allocator;
};

class Texture {
public:
Texture (textureCreateInfo info);
~Texture();

void loadImage        (const char* filename) ;
void uploadDescriptor (VkDescriptorSet set);
void copyBufferToImage(VkCommandBuffer command);

VkImageView getImageView()   const { return textureImageView; }
VkSampler   getSampler()     const { return textureSampler;   }

private:
VkDevice                device;
MemoryAllocator&        allocator;
VkImage                 textureImage;
VkImageView             textureImageView;
Allocation              textureMemory;
VkSampler               textureSampler;
std::unique_ptr<Buffer> buffer;
uint32_t                width; 
uint32_t                height;
// 1. Create staging buffer and copy pixel data to it (omitted here, add as needed)
// 2. Create VkImage with VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
// 3. Allocate memory, bind to image
// 4. Transition image layout, copy buffer to image, transition to shader read layout
// For brevity, assume helper functions exist for above steps:
//createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
//            VK_IMAGE_TILING_OPTIMAL,
//            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
//            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//            textureImage, textureImageMemory);
// Copy pixels data to image via staging buffer and command buffers (helper function needed)
// copyBufferToImage(pixels, texWidth, texHeight);
void createImage();
void createImageView();
};
#endif //