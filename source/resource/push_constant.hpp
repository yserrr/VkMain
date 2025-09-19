//
// Created by ljh on 25. 9. 18..
//

#ifndef MYPROJECT_PUSH_CONSTANT_HPP
#define MYPROJECT_PUSH_CONSTANT_HPP
#include "glm/glm.hpp"

struct PushConstant{
  glm::mat4 model;
  glm::vec4 color;
  uint32_t bindlessIndex = 0;
};

using MeshConstant = PushConstant;
#endif //MYPROJECT_PUSH_CONSTANT_HPP