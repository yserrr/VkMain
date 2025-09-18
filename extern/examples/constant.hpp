//
// Created by ljh on 25. 8. 24..
//

#ifndef MYPROJECT_CONSTANT_HPP
#define MYPROJECT_CONSTANT_HPP
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
namespace gpu
{
  struct constant {
    glm::vec4 data1;
    glm::vec4 data2;
    glm::vec4 data3;
    glm::vec4 data4;
  };
}


class Constant{
  public:
  Constant();
  ~Constant();
private:
  std::vector<gpu::constant> constants_;
  VkPushConstantRange pushConstantRangeCache_{};
  uint32_t sizeInBytes_;



};

// VkPushConstantRange pushConstantRange{};
//    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
//    pushConstantRange.offset = 0;
//    //pushConstantRange.size = sizeof(MaterialType);
//    //setting materal for rendereing material constant setting
//
//    // MaterialType materialData = {};
//    // materialData.albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
//    // materialData.params = glm::vec4(0.5f, 0.3f, 1.0f, 0.0f);  // metallic, roughness, ao, padding
//    // materialData.flags = (1 << 0); // 예: useAlbedoMap 플래그
//
//    // vkCmdPushConstants(
//    // command,
//    // layout,
//    // VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
//    // 0,
//    // sizeof(MaterialType),
//    // &materialData

//    // vkCmdPushConstants(
//    // command,
//    // layout,
//    // VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
//    // 0,
//    // sizeof(MaterialType),
//    // &materialData
//    //);

#endif //MYPROJECT_CONSTANT_HPP