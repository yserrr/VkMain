#include "resource_manager.hpp"
#include "camera_cfg.hpp"
#include "descriptor_layout_config.hpp"

ResourceManager::ResourceManager(const ResourceManagerCreateInfo &info) :
  device_(info.device),
  allocator_(*info.allocator),
  maxInflight_(info.maxInflight),
  descriptorUploader_(info.device)
{
  vkGetPhysicalDeviceProperties(allocator_.getPhysicalDevice(), &physicalDeviceProperties);
  minUboAlign_    = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
  samplerBuilder_ = std::make_unique<SamplerBuilder>(device_);

  lightManager = std::make_unique<LightManager>(device_, allocator_);
  Light light;
  light.position  = glm::vec3(1.0, 1.0, 2.0);
  light.type      = 1;
  light.direction = glm::vec3(0, 0, 0);
  light.intensity = 0.2;
  light.color     = glm::vec3(0.3, 0.2, 0.5);
  lightManager->addLight(light);
  lightManager->uploadData();
  std::unique_ptr<Material> base = std::make_unique<Material>();
  base->name = "base";
  materials_[base->name] = std::move(base);
  spdlog::info("add lights");
  CamCI cam_ci{};
  cam_ci.fov         = glm::radians(350.0f);
  cam_ci.aspectRatio = 2400 / 1200;
  cam_ci.nearPlane   = 1.0;
  cam_ci.farPlane    = 256.0;
  camera             = std::make_unique<Camera>(cam_ci);

  descriptorManager   = std::make_unique<DescriptorManager>(allocator_.getDevice());
  bindlessDescirptor_ = descriptorManager->bindlessSet();

  spdlog::info("allocate descriptors");

  mainCamBuffers_.resize(info.maxInflight);
  uboBuilder_ = std::make_shared<UBOBuilder>(allocator_, BufferType::UNIFORM, AccessPolicy::HostPreferred);

  for (int i = 0; i < maxInflight_; i++)
  {
    auto buffer                      = uboBuilder_->buildStaticBuffer(sizeof(camera->ubo));
    VkDescriptorSet set              = descriptorManager->allocateSet();
    mainCamBuffers_[i].descriptorSet = set;
    mainCamBuffers_[i].bindingIndex  = gpu::CAMERA_BINDING;
    mainCamBuffers_[i].buffer        = *buffer->getBuffer();

    descriptorUploader_.UploadUboSet(*buffer->getBuffer(),
                                     sizeof(camera->ubo),
                                     set,
                                     0);
    lightManager->uploadDescriptor(set);
    camBuffers.push_back(buffer);
  }
}

void ResourceManager::updateDescriptorSet(uint32_t currentFrame)
{
  camBuffers[currentFrame]->loadData(&(camera->ubo), sizeof(camera->ubo));
}

ResourceManager::~ResourceManager() = default;