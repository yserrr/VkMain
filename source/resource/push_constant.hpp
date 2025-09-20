//
// Created by ljh on 25. 9. 18..
//

#ifndef MYPROJECT_PUSH_CONSTANT_HPP
#define MYPROJECT_PUSH_CONSTANT_HPP
#include "glm/glm.hpp"


struct MaterialConstant
{
  glm::mat4 modelMatrix  = glm::mat4(1.0f);
  glm::vec4 color        = glm::vec4(1.0f);
  glm::vec3 camPos       = glm::vec3(0.0f);
  int albedoTextureIndex = 0;
  uint32_t normalTextureIndex  = 0;
  uint32_t metalicTextureIndex = 0;
  glm::vec3 emissiveColor      = glm::vec3(0.0f);
  float metallic               = 0.0f;
  float roughness              = 1.0f;
  float ao                     = 1.0f;
  float emission               = 0.0f;
  float normalScale            = 1.0f;
  float alphaCutoff            = 0.0f;
};
#endif //MYPROJECT_PUSH_CONSTANT_HPP