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
#include <camera_cfg.hpp>
#include "brush.hpp"
#include "dyn_mesh.hpp"


struct ZBuffer{
  std::vector<float> depth;
  std::unordered_map<uint32_t,uint32_t> indices;
};

class Painter{};

class Sculptor : Painter{
  friend class Engine;
  friend class UIRenderer;
  friend class EventManager;

public:
  Sculptor()  = default;
  ~Sculptor() = default;
  void stroke(Ray ray);
  bool RayIntersectTriangle(const glm::vec3 &orig,
                            const glm::vec3 &dir,
                            const glm::vec3 &v0,
                            const glm::vec3 &v1,
                            const glm::vec3 &v2,
                            float &t,
                            float &u,
                            float &v);
  uint64_t sculptMidPoint(uint32_t i0, uint32_t i1);
  uint64_t getCash(uint32_t a, uint32_t b);
  bool castRayToMesh(const glm::vec3 &rayOrig, const glm::vec3 &rayDir);
  void subdivideMesh();

private:
  std::unordered_map<uint64_t, uint32_t> midpointCache;
  bool isDynamic= true;
  bool dirty_   = false;
  bool hitAny   = false;
  float hit_t   = FLT_MAX;
  int hitTriIdx = -1;
  Brush brush{
  5.0,
  0.3
  };
  DynMesh *mesh_;
};

#endif //MYPROJECT_SCULPTOR_HPP