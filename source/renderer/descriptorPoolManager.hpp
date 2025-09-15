#include<common.hpp>
#include<descriptor_pool.hpp>
#ifndef DESCRIPTORPOOLMANAGER_HPP
#define DESCRIPTORPOOLMANAGER_HPP
#define CAM_MAX     5
#define TEXTURE_MAX 5
#define LIGHT_MAX   5

//control descriptor layout for setting descriptor Pool
class DescriptorManager{
public:
  DescriptorManager(VkDevice device)
    : device(device)
  {
//create Descriptor Managers layout for vertex Stage
//using uniform buffe to camera or global uniform
    VkDescriptorSetLayoutBinding descriptorLayoutBinding{};
    descriptorLayoutBinding.binding            = 0;
    descriptorLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorLayoutBinding.descriptorCount    = 1;
    descriptorLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorLayoutBinding.pImmutableSamplers = nullptr;
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings    = &descriptorLayoutBinding;
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &vertexLayout) != VK_SUCCESS)
      throw std::runtime_error("failed to create descriptor set layout!");
//for texture
//using combined Image with sampler type
    VkDescriptorSetLayoutBinding fragmentBinding{};
    fragmentBinding.binding            = 0;
    fragmentBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    fragmentBinding.descriptorCount    = 1;
    fragmentBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentBinding.pImmutableSamplers = nullptr;
    VkDescriptorSetLayoutCreateInfo fragInfo{};
    fragInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    fragInfo.bindingCount = 1;
    fragInfo.pBindings    = &fragmentBinding;
    if (vkCreateDescriptorSetLayout(device, &fragInfo, nullptr, &textureLayout) != VK_SUCCESS)
      throw std::runtime_error("failed to create descriptor set layout!");
//light material : set 0 , biniding 1
    VkDescriptorSetLayoutBinding lightBinding{};
    lightBinding.binding            = 0;
    lightBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightBinding.descriptorCount    = 1;
    lightBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    lightBinding.pImmutableSamplers = nullptr;
    VkDescriptorSetLayoutCreateInfo lightInfo{};
    lightInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    lightInfo.bindingCount = 1;
    lightInfo.pBindings    = &lightBinding;
    if (vkCreateDescriptorSetLayout(device, &lightInfo, nullptr, &lightLayout) != VK_SUCCESS)
      throw std::runtime_error("failed to create descriptor set layout!");
    layouts = {vertexLayout, textureLayout, lightLayout};

    cameraPool  = std::make_unique<CameraDescriptorPool>(device, vertexLayout);
    texturePool = std::make_unique<TextureDescriptorPool>(device, textureLayout);
    lightPool   = std::make_unique<LightDescriptorPool>(device, lightLayout);
    cameraPool->createPool(CAM_MAX); //max camera sets
    texturePool->createPool(TEXTURE_MAX);
    lightPool->createPool(LIGHT_MAX);
  }

  ~DescriptorManager()
  {
    if (vertexLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(device, vertexLayout, nullptr);
    if (textureLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(device, textureLayout, nullptr);
    if (lightLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(device, lightLayout, nullptr);
  }

//allocate camera set
  void allocateDescriptor()
  {
    descriptors[0] = cameraPool->allocateSet();
    descriptors[1] = texturePool->allocateSet();
    descriptors[2] = lightPool->allocateSet();
  }

  std::vector<VkDescriptorSetLayout> *getLayouts()
  {
    return &layouts;
  }

  std::array<VkDescriptorSet, 3> getSets()
  {
    return descriptors;
  }

  VkDescriptorSetLayout getVert()
  {
    return vertexLayout;
  }

  VkDescriptorSetLayout getFrag()
  {
    return textureLayout;
  }

  VkDescriptorSetLayout getLight()
  {
    return lightLayout;
  }

//VkDescriptorSet getLightSet (uint32_t frameIndex);
//VkDescriptorSet getObjectSet(uint32_t frameIndex, uint32_t objectID);
private:
  VkDevice device;
  std::unique_ptr<CameraDescriptorPool> cameraPool;
  std::unique_ptr<TextureDescriptorPool> texturePool;
  std::unique_ptr<LightDescriptorPool> lightPool;
  VkDescriptorSetLayout vertexLayout;  //temp cam
  VkDescriptorSetLayout textureLayout; //temp light ;
  VkDescriptorSetLayout lightLayout;   //both
  std::vector<VkDescriptorSetLayout> layouts;
  std::array<VkDescriptorSet, 3> descriptors;
  VkDescriptorSet cameraSet;
  VkDescriptorSet textureSet;
  VkDescriptorSet lightSet;
};

#endif //DESCRIPTORMANAGER_HPP