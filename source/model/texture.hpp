#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <common.hpp> 
#include <../resource/sampler_pool.hpp>
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
void createImage();
void createImageView();
};
#endif //