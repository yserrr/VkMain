#include "sculptor.hpp"

#include "spdlog/details/os.h"

void Sculptor::stroke(Ray ray)
{
  spdlog::info("[Sculptor] gen ray direction: {} {} {}", ray.direction.x, ray.direction.y, ray.direction.z);
  if (castRayToMesh(ray.origin, ray.direction))
  {
    glm::vec3 hitPoint = hit_t * ray.direction + ray.origin;
    for (auto &v: mesh_->vertices)
    {
      float dist = glm::distance(hitPoint, v.position);
      if (dist < brush.radius)
      {
        // hit point 기준을 바탕으로 gen
        //VertexAll vertex{};
        //vertex.position = (hitPoint + v.position) / 2.0f;
        //vertex.normal =
        //mesh_->indices.push_back()
        float falloff = 1.0f - (dist / brush.radius);
        v.position += v.normal * brush.strength * falloff;
      }
    }
    mesh_->reNomalCompute();
    dirty_ = true;
  }
  return;
}

bool Sculptor::RayIntersectTriangle(const glm::vec3 &orig,
                                    const glm::vec3 &dir,
                                    const glm::vec3 &v0,
                                    const glm::vec3 &v1,
                                    const glm::vec3 &v2,
                                    float &t,
                                    float &u,
                                    float &v)
{
  const float THREAD_HOLD = 1e-10f;

  glm::vec3 edge1 = v1 - v0;
  glm::vec3 edge2 = v2 - v0;
  glm::vec3 h     = glm::cross(dir, edge2);
  float a         = glm::dot(edge1, h);

  if (fabs(a) < THREAD_HOLD) return false;

  float f     = 1.0f / a;
  glm::vec3 s = orig - v0;

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

bool Sculptor::castRayToMesh(const glm::vec3 &rayOrig, const glm::vec3 &rayDir)
{
  hitAny    = false;
  hit_t     = FLT_MAX;
  hitTriIdx = -1;
  #pragma omp parallel for
  for (size_t i = 0; i < mesh_->indices.size(); i += 3)
  {
    glm::vec3 v0 = mesh_->vertices[mesh_->indices[i]].position;
    glm::vec3 v1 = mesh_->vertices[mesh_->indices[i + 1]].position;
    glm::vec3 v2 = mesh_->vertices[mesh_->indices[i + 2]].position;

    float t, u, v;
    if (RayIntersectTriangle(rayOrig, rayDir, v0, v1, v2, t, u, v))
    {
      if (t < hit_t)
      {
        hit_t     = t;
        hitTriIdx = i / 3;
        hitAny    = true;
      }
    }
  }
  if (hitAny)
  {
    std::cout << "[Sculptor] Hit triangle: {} , t: {}" << std::endl;
  } else
  {
    spdlog::info("[Sculptor] No hit");
  }
  return hitAny;
}

uint64_t Sculptor::sculptMidPoint(uint32_t i0, uint32_t i1)
{
  uint64_t key = getCash(i0, i1);
  if (midpointCache.count(key))
  {
    return midpointCache[key];
  }
  glm::vec3 p0 = mesh_->vertices[i0].position;
  glm::vec3 p1 = mesh_->vertices[i1].position;
  glm::vec3 n0 = mesh_->vertices[i0].normal;
  glm::vec3 n1 = mesh_->vertices[i1].normal;

  VertexAll mid;
  mid.position = 0.5f * (p0 + p1);
  mid.normal   = glm::normalize(n0 + n1);

  uint32_t newIndex = static_cast<uint32_t>(mesh_->vertices.size());
  mesh_->vertices.push_back(mid);
  midpointCache[key] = newIndex;
  return newIndex;
}

uint64_t Sculptor::getCash(uint32_t a, uint32_t b)
{
  if (a > b) std::swap(a, b);
  return (static_cast<uint64_t>(a) << 32) | b;
}

void Sculptor::subdivideMesh()
{
  std::vector<uint32_t> newIndices;
  for (size_t i = 0; i < mesh_->indices.size(); i += 3)
  {
    uint32_t i0 = mesh_->indices[i];
    uint32_t i1 = mesh_->indices[i + 1];
    uint32_t i2 = mesh_->indices[i + 2];

    uint32_t m0 = sculptMidPoint(i0, i1);
    uint32_t m1 = sculptMidPoint(i1, i2);
    uint32_t m2 = sculptMidPoint(i2, i0);

    newIndices.insert(newIndices.end(), {i0, m0, m2});
    newIndices.insert(newIndices.end(), {i1, m1, m0});
    newIndices.insert(newIndices.end(), {i2, m2, m1});
    newIndices.insert(newIndices.end(), {m0, m1, m2});
  }
  mesh_->indices = std::move(newIndices);
}