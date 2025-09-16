
#ifndef ASSETMANAGER_HPP
#define ASSETMANAGER_HPP

#include <texture.hpp>
#include <mesh.hpp>
//#include <material.hpp>
#include <light.hpp>
#include <common.hpp>
#include <descriptorPoolManager.hpp>
#include <importer.hpp>
#include <material.hpp>
#include <camera.hpp>

struct ResourceManagerCreateInfo{
  VkDevice         device;
  MemoryAllocator *allocator;
};
struct BufferContext{
  VkBuffer buffer = VK_NULL_HANDLE;
  Allocation allocation {};

};


using VkBindlessDescriptor = VkDescriptorSet;
class ResourceManager{
public:
  ResourceManager(const ResourceManagerCreateInfo &info) :
    device(info.device),
    allocator(*info.allocator) {}
  ~ResourceManager() {}

  Mesh loadMesh();
  void uploadDescriptors(std::array<VkDescriptorSet, 3> sets);
  void setUp(VkCommandBuffer command);
  void setCamera(VkExtent2D extent);
  void setTexture();
  void setLight();

  Camera *getCamera();

  std::unique_ptr<Mesh> mesh;

  ImporterEx *importer;

private:
  VkDevice                        device;
  MemoryAllocator &               allocator;
  std::unique_ptr<SamplerBuilder> sampler;
  std::shared_ptr<VulkanTexture>  texture;
  std::shared_ptr<VulkanTexture>  nomal;
  std::unique_ptr<LightManager>  lightManager;

  std::unique_ptr<Camera>        camera;
  std::array<VkDescriptorSet, 3> descriptors;
};

#endif