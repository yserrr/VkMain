#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER
#include <texture.hpp>
#include <../sculptor/dyn_mesh.hpp>
#include <light.hpp>
#include <common.hpp>
#include <../resource/descriptor_manager.hpp>
#include <importer.hpp>
#include <material.hpp>
#include <camera.hpp>
#include "buffer_builder.hpp"
#include <unordered_map>
#include "descriptor_uploader.hpp"
#include "push_constant.hpp"

using Key = std::string;


struct ResourceManagerCreateInfo{
  VkDevice device            = VK_NULL_HANDLE;
  MemoryAllocator *allocator = nullptr;
  uint32_t maxInflight       = 3;
};

class ResourceManager{
  friend class Engine;
  friend class SceneRenderer;
  friend class UIRenderer;

public:
  ResourceManager(const ResourceManagerCreateInfo &info);
  ~ResourceManager();
  void updateDescriptorSet(uint32_t currentFrame);
  void uploadDescriptors();
  void uploadMesh(VkCommandBuffer command, std::string path);
  void uploadTexture(VkCommandBuffer command, std::string path);
  void setTexture();
  VulkanTexture* getTexture(Key path);
  void setLight();
  std::vector<VulkanTexture*> uploadedTexture;
  std::unique_ptr<DescriptorManager> descriptorManager;
  std::vector<std::shared_ptr<StaticBuffer> > camBuffers;
  DynMesh* currentMesh = nullptr;
  Camera *getCamera();

  ImporterEx importer_;
  uint32_t maxInflight_ = 3;

private:
  VkDevice device_;
  MemoryAllocator &allocator_;
  VkDeviceSize minUboAlign_;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  DescriptorUploader descriptorUploader_;
  VkBindlessDescriptor bindlessDescirptor_;
  std::unordered_map<Key, std::unique_ptr<DynMesh> > meshes_;
  std::unordered_map<Key, std::unique_ptr<VulkanTexture> > textures_;
  std::shared_ptr<UBOBuilder> uboBuilder_;
  std::vector<BufferContext> mainCamBuffers_;
  std::unique_ptr<SamplerBuilder> samplerBuilder_;
  std::shared_ptr<VulkanTexture> nomal;
  std::unique_ptr<LightManager> lightManager;
  PushConstant pushConstant_{};
  std::unique_ptr<Camera> camera;
};

#endif