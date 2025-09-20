#ifndef MESH_HPP
#define MESH_HPP
#include <../model/vertex.hpp>
#include <static_buffer.hpp>

#include "material.hpp"
#include "push_constant.hpp"

// tool box mesh -> simple
// rendering resource -> batch style
// don't optimize temp resource
/// dynamic mesh:
/// sub device -> allocate need to setting with dencity
/// must setting
struct Mesh{
  Mesh(const std::vector<VertexAll> &vertices,
       const std::vector<uint32_t> &indices,
       MemoryAllocator &allocator);
  ~Mesh();
  void dynMeshUpdate(VkCommandBuffer commandBuffer);
  void copyBuffer(VkCommandBuffer commandBuffer) const;
  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer) const;
  const std::vector<VertexAll> &getVertices() const;
  const std::vector<uint32_t> &getIndices() const;
  void recenterMesh();
  void reNomalCompute();
  std::string name;
  std::unique_ptr<StaticBuffer> vertexBuffer;
  std::unique_ptr<StaticBuffer> indexBuffer;
  std::vector<VertexAll> vertices;
  std::vector<uint32_t> indices;
  MemoryAllocator &allocator;
  VkDeviceSize vertexSize;
  VkDeviceSize indiceSize;
};

#endif //MESH_HPP