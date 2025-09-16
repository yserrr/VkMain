//
// Created by ljh on 25. 8. 31..
//

#ifndef MYPROJECT_SCULPTOR_HPP
#define MYPROJECT_SCULPTOR_HPP

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <mesh_sub.hpp>

namespace gpu
{
  struct stroke{
    uint32_t index;
    uint32_t radious;
    glm::vec3 center;
  };
}

/// todo:
///   this class for drowing mesh
///   if called -> must reference current mesh
///    ssbo  <> vertex -> matching relation
///    if called -> push constant on weight to ssbo update
///    -> compute shader get the weight
///    vertex shader -> get vertex on updated weight
///    ray-casting need to model view matrix transform
///    -> all control with interacion and current state of camera need
///

class Sculptor{
public:
  Sculptor();
  ~Sculptor();
  void stroke(Ray ray, bool inner);

private:
  float currentBrushSize;
  float currentStroke_;

};


#endif //MYPROJECT_SCULPTOR_HPP
