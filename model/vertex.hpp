#ifndef VERTEX_HPP
#define VERTEX_HPP

#include<common.hpp>
struct Vertex {
    glm::vec3 position;      // location = 0
    glm::vec3 normal;        // location = 1
    glm::vec2 uv;            // location = 2
    glm::vec3 tangent;       // location = 3
    glm::vec3 bitangent;     // location = 4
    glm::vec4 color;         // location = 5
    glm::ivec4 boneIndices;       // location = 6
    glm::vec4 boneWeights;        // location = 7
};

#endif 