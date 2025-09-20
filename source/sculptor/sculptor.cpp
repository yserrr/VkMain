#include "sculptor.hpp"
#include "spdlog/details/os.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
constexpr float THREAD_HOLD = 1e-6f;

Sculptor::Sculptor(Model *model) : mesh(model->mesh) {}

bool Sculptor::stroke(glm::vec3 strokeDir)
{
  spdlog::debug("[Sculptor] gen ray direction: {} {} {}", strokeDir.x, strokeDir.y, strokeDir.z);
  if (castRayToMesh( strokeDir))
  {
    glm::vec3 hitPoint = hit_t * strokeDir;
    for (auto &v: mesh->vertices)
    {
      float dist = glm::distance(hitPoint, v.position);
      if (dist < brush.radius)
      {
        // hit point 기준을 바탕으로 gen
        //VertexAll vertex{};
        //vertex.position = (hitPoint + v.position) / 2.0f;
        //vertex.normal =
        //mesh->indices.push_back()
        float falloff = 1.0f - (dist / brush.radius);
        v.position += v.normal * brush.strength * falloff;
      }
    }
    mesh->reNomalCompute();
    return true;
  }
  return false;
}

bool Sculptor::RayIntersectTriangle(const glm::vec3 &dir,
                                    const glm::vec3 &v0,
                                    const glm::vec3 &v1,
                                    const glm::vec3 &v2,
                                    float &t,
                                    float &u,
                                    float &v)
{
  glm::vec3 edge1 = v1 - v0;
  glm::vec3 edge2 = v2 - v0;

  glm::vec3 h = glm::cross(dir, edge2);
  float a     = glm::dot(edge1, h);

  if (fabs(a) < THREAD_HOLD) return false;

  float f     = 1.0f / a;
  glm::vec3 s = -v0;

  u = f * glm::dot(s, h);
  if (u < 0.0f or u > 1.0f) return false;

  glm::vec3 q = glm::cross(s, edge1);

  v = f * glm::dot(dir, q);
  if (v < 0.0f or u + v > 1.0f) return false;

  t = f * glm::dot(edge2, q);
  if (t > THREAD_HOLD)
    return true;
  return false;
}

bool Sculptor::castRayToMesh(const glm::vec3 &rayDir)
{
  hitAny         = false;
  hit_t          = FLT_MAX;
  hitTriangleIdx = -1;
  #pragma omp parallel for
  for (size_t i = 0; i < mesh->indices.size(); i += 3)
  {
    glm::vec3 v0 = mesh->vertices[mesh->indices[i]].position;
    glm::vec3 v1 = mesh->vertices[mesh->indices[i + 1]].position;
    glm::vec3 v2 = mesh->vertices[mesh->indices[i + 2]].position;
    float t, u, v;
    if (RayIntersectTriangle(rayDir, v0, v1, v2, t, u, v))
    {
      if (t < hit_t)
      {
        hit_t          = t;
        hitTriangleIdx = i / 3;
        hitAny         = true;
      }
    }
  }
  if (hitAny)
  {
    spdlog::info("[Sculptor] Hit triangle: {} , t: {}", hitTriangleIdx, hit_t);
  } else
  {
    spdlog::info("[Sculptor] No hit");
  }
  return hitAny;
}

uint64_t Sculptor::sculptMidPoint(uint32_t i0, uint32_t i1)
{
  uint64_t key = getVertexCash(i0, i1);
  if (midpointCache.count(key))
  {
    return midpointCache[key];
  }
  glm::vec3 p0 = mesh->vertices[i0].position;
  glm::vec3 p1 = mesh->vertices[i1].position;
  glm::vec3 n0 = mesh->vertices[i0].normal;
  glm::vec3 n1 = mesh->vertices[i1].normal;

  VertexAll mid;
  mid.position = 0.5f * (p0 + p1);
  mid.normal   = glm::normalize(n0 + n1);

  uint32_t newIndex = static_cast<uint32_t>(mesh->vertices.size());
  mesh->vertices.push_back(mid);
  midpointCache[key] = newIndex;
  return newIndex;
}

uint64_t Sculptor::getVertexCash(uint32_t a, uint32_t b)
{
  if (a > b) std::swap(a, b);
  return (static_cast<uint64_t>(a) << 32) | b;
}

void Sculptor::subdivideMesh()
{
  std::vector<uint32_t> newIndices;
  for (size_t i = 0; i < mesh->indices.size(); i += 3)
  {
    uint32_t i0 = mesh->indices[i];
    uint32_t i1 = mesh->indices[i + 1];
    uint32_t i2 = mesh->indices[i + 2];

    uint32_t m0 = sculptMidPoint(i0, i1);
    uint32_t m1 = sculptMidPoint(i1, i2);
    uint32_t m2 = sculptMidPoint(i2, i0);

    newIndices.insert(newIndices.end(), {i0, m0, m2});
    newIndices.insert(newIndices.end(), {i1, m1, m0});
    newIndices.insert(newIndices.end(), {i2, m2, m1});
    newIndices.insert(newIndices.end(), {m0, m1, m2});
  }
  mesh->indices = std::move(newIndices);
}