// Mesh.hpp
#ifndef MESH_HPP
#define MESH_HPP

#include <vertex.hpp>
#include <static_buffer.hpp>\

// tool box mesh -> simple
// rendering resource -> batch style
// don't optimize temp resource

class Mesh{
  friend class ResourceManager;
public:
  Mesh(const std::vector<VertexAll> &vertices,
       const std::vector<uint32_t> &indices,
       MemoryAllocator &allocator);

  ~Mesh();

  void copyBuffer(VkCommandBuffer commandBuffer) const;
  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer) const;

  const std::vector<VertexAll> &getVertices() const;

  const std::vector<uint32_t> &getIndices() const;

  void recenterMesh();

private:
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