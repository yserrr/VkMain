#ifndef MODEL_HPP
#define MODEL_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <mesh.hpp>
#include <material.hpp>
#include "mesh_sub.hpp"
#include <glm/gtc/quaternion.hpp>
// static call ->batch 만들기
// sink구조 생각하면 될듯 ?
struct Transform{
  glm::quat rotation{};
  glm::vec3 position;
  glm::vec3 scale;
};

struct Model{
  Mesh *mesh = nullptr;
  Material *material;
  Submesh submesh{};
  MaterialConstant constant{};
  Transform transform;
  std::string name;
};

#endif