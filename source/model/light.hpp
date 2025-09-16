#include <glm/glm.hpp>
#include <common.hpp>
#include <buffer.hpp>
#define MAX_LIGHTS 16
#ifndef LIGHT_HPP
#define LIGHT_HPP

enum class LightType{
  Directional = 0,
  Point       = 1,
  Spot        = 2
};

struct Light{
  glm::vec3 position;  // for point/spot
  int       type;      // casted from LightType
  glm::vec3 direction; // for directional/spot
  float     angle;     // for spot light only
  glm::vec3 color;
  float     intensity;
};

struct GPULight{
  glm::vec4 position;  // xyz: 위치 / w: 타입
  glm::vec4 direction; // xyz: 방향 / w: cutoff(cos값)
  glm::vec4 color;     // rgb: 색 /   a: intensity
};

struct lightUBO{
  GPULight        lights[MAX_LIGHTS];
  alignas(16) int lightCount;
};

class LightManager{
public:
  LightManager(VkDevice device, MemoryAllocator &allocator)
    :
    device(device),
    allocator(allocator)
  {
///
    ubo.lightCount = 0;
    buffer         = std::make_unique<Buffer>(allocator, sizeof(lightUBO), BufferType::Uniform);
    buffer->createUniformBuffer();
  }

  void addLight(const Light &light)
  {
    if (ubo.lightCount >= MAX_LIGHTS) return;
    GPULight &gpu = ubo.lights[ubo.lightCount++];
    gpu.position  = glm::vec4(light.position, static_cast<float>(light.type));
    gpu.direction = glm::vec4(glm::normalize(light.direction), light.angle);
    gpu.color     = glm::vec4(light.color, light.intensity);
  }

  void uploadDescriptor(VkDescriptorSet set)
  {
    VkDescriptorBufferInfo lightBufferInfo{};
    lightBufferInfo.buffer = buffer->getBuffer()[0];
    lightBufferInfo.offset = 0;
    lightBufferInfo.range  = sizeof(lightUBO);

    VkWriteDescriptorSet lightWrite{};
    lightWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    lightWrite.dstSet          = set;
    lightWrite.dstBinding      = 0; //layout space
    lightWrite.dstArrayElement = 0;
    lightWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightWrite.descriptorCount = 1;
    lightWrite.pBufferInfo     = &lightBufferInfo;
    vkUpdateDescriptorSets(device, 1, &lightWrite, 0, nullptr);
  }

  void uploadData()
  {
    buffer->loadData(&ubo, sizeof(ubo));
  }

private:
  VkDevice                device;
  MemoryAllocator &       allocator;
  std::unique_ptr<Buffer> buffer;
  lightUBO                ubo;
};

#endif