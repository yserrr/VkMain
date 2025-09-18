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

  // default light
  lightManager = std::make_unique<LightManager>(device_, allocator_);
  Light light;
  light.position  = glm::vec3(1.0, 1.0, 2.0);
  light.type      = 1;
  light.direction = glm::vec3(0, 0, 0);
  light.intensity = 0.2;
  light.color     = glm::vec3(1, 0.7, 0.5);
  lightManager->addLight(light);
  Light light2;
  light2.position  = glm::vec3(1.0, 1.0, 10.0);
  light2.type      = 1;
  light2.direction = glm::vec3(0, 0, 0);
  light2.intensity = 0.6;
  light.color      = glm::vec3(0, 0.7, 0.5);
  lightManager->addLight(light2);
  lightManager->uploadData();
  spdlog::info("add lights");

  camCreateInfo camInfo{};
  camInfo.fov         = glm::radians(350.0f);
  camInfo.aspectRatio = 2400 / 1200;
  camInfo.nearPlane   = 0.1f;
  camInfo.farPlane    = 10000.0;
  camInfo.allocator   = &allocator_;

  descriptorManager = std::make_unique<DescriptorManager>(allocator_.getDevice());

  bindlessDescirptor_ = descriptorManager->bindlessSet();

  spdlog::info("allocate descriptors");

  mainCamBuffers_.resize(info.maxInflight);
  camera      = std::make_unique<Camera>(camInfo);
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