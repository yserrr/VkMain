#ifndef MYPROJECT_MATERIAL_PARAM_HPP
#define MYPROJECT_MATERIAL_PARAM_HPP
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <vector>
namespace cpu
{
  struct material_index{
    //index -> other textures
    uint32_t flags             = 0;
    uint32_t albedoIndex       = 0;
    uint32_t normalIndex       = 0;
    uint32_t metallicTexIndex  = 0;
    uint32_t roughnessTexIndex = 0;
    uint32_t aoTexIndex        = 0;
    uint32_t emissionTexIndex  = 0;
    uint32_t padding           = 0;
  };

  struct material_cfg{
    glm::vec4 baseColor     = glm::vec4(1.0f);
    glm::vec3 emissiveColor = glm::vec3(0.0f);
    float metallic          = 0.0f; //intense
    float roughness         = 1.0f;
    float ao                = 1.0f;
    float emission          = 0.0f;
    float normalScale       = 1.0f;
    float alphaCutoff       = 0.5f;
    bool alphaBlend         = false;
    bool doubleSided        = false;
  };

  struct material{
    material_cfg cfg{};
    material_index textureIndex;
    bool dirty;
  };
}

namespace gpu
{
  struct alignas(16) material{
    cpu::material_cfg cfg;
    cpu::material_index textureIndex;
    uint32_t size = 0;
    uint32_t padding1;
    uint32_t padding2;
    uint32_t padding3;
  };
};


#endif //MYPROJECT_MATERIAL_PARAM_HPP
