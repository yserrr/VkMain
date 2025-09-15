#include<common.hpp> 
#ifndef DESCRIPTORPOOL_HPP
#define DESCRIPTORPOOL_HPP 
///after must make virtual class 
enum class descriptorType{
    Texture,
    Camera, 
    light
};
class DescriptorPool{
public:
DescriptorPool(VkDevice              device,
               VkDescriptorSetLayout layout)
: device(device),
  layout(layout), 
  descriptorPool(VK_NULL_HANDLE){}
virtual ~DescriptorPool() {
if (descriptorPool != VK_NULL_HANDLE)
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}
// Set 할당 시 내부에서 vector에 저장 및 관리 
virtual void createPool(uint32_t count)= 0;
      VkDescriptorSet               allocateSet();
      VkDescriptorPool              getDescriptorPool() const {return descriptorPool; } 
const std::vector<VkDescriptorSet>  getDescriptorSet()  const {return allocatedSets;  }
      VkDescriptorSet getSetAt(uint32_t index) const {return allocatedSets.at(index); }
protected: //상속 대상이 접근 가능해야함
std::vector<VkDescriptorSet>  allocatedSets;
VkDevice                      device; 
VkDescriptorPool              descriptorPool;  
VkDescriptorSetLayout         layout;
};





class CameraDescriptorPool: public DescriptorPool{
public:
CameraDescriptorPool(VkDevice device, VkDescriptorSetLayout layout)
:DescriptorPool(device,layout){}
void createPool(uint32_t count) override; 
};

class TextureDescriptorPool: public DescriptorPool{ 
public:
TextureDescriptorPool(VkDevice device, VkDescriptorSetLayout layout)
:DescriptorPool(device,layout){}
void createPool(uint32_t count) override;
};

class LightDescriptorPool: public DescriptorPool{
public:
LightDescriptorPool(VkDevice device, VkDescriptorSetLayout layout)
:DescriptorPool(device,layout){}
void createPool(uint32_t count) override;
};


#endif 