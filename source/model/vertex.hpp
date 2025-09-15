#ifndef VERTEX_HPP
#define VERTEX_HPP

#include<common.hpp>

enum class VertexType:uint32_t{
  PC,
  PUVN,
  PUVNTC,
  ALL
};

struct VertexPC{
  glm::vec3 position;
  glm::vec3 color;
};

struct VertexPUVN{
  glm::vec3 position;
  glm::vec2 uv;
  glm::vec3 normal;
};

struct VertexPUVNTC{
  glm::vec3 position;
  glm::vec2 uv;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec3 bitangent;
  glm::vec3 color;
};

struct VertexAll{
  glm::vec3 position;     // location = 0
  glm::vec3 normal;       // location = 1
  glm::vec2 uv;           // location = 2
  glm::vec3 tangent;      // location = 3
  glm::vec3 bitangent;    // location = 4
  glm::vec4 color;        // location = 5
  glm::ivec4 boneIndices; // location = 6
  glm::vec4 boneWeights;  // location = 7
};

#endif