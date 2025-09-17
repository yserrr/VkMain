#include "mesh.hpp"

Mesh::Mesh(const std::vector<VertexAll> &vertices,
           const std::vector<uint32_t> &indices,
           MemoryAllocator &allocator) : vertices(vertices), indices(indices), allocator(allocator)
{
  recenterMesh();
  vertexSize   = (sizeof(vertices[0])) * vertices.size();
  indiceSize   = sizeof(indices[0]) * indices.size();
  vertexBuffer = std::make_unique<StaticBuffer>(allocator, vertexSize, BufferType::VERTEX);
  indexBuffer  = std::make_unique<StaticBuffer>(allocator, indiceSize, BufferType::INDEX);
  vertexBuffer->getStagingBuffer(vertices.data());
  indexBuffer->getStagingBuffer(indices.data());
  vertexBuffer->createMainBuffer();
  indexBuffer->createMainBuffer();
}

void Mesh::copyBuffer(VkCommandBuffer commandBuffer) const
{
  vertexBuffer->copyBuffer(commandBuffer);
  indexBuffer->copyBuffer(commandBuffer);
}

void Mesh::bind(VkCommandBuffer commandBuffer)
{
  VkDeviceSize offsets[] = {0};
  VkBuffer vertexBufs[]  = {*(vertexBuffer->getBuffer())};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBufs, offsets);
  vkCmdBindIndexBuffer(commandBuffer, *(indexBuffer->getBuffer()), 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::draw(VkCommandBuffer commandBuffer) const
{
  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

Mesh::~Mesh() {}

const std::vector<VertexAll> &Mesh::getVertices() const
{
  return vertices;
}

const std::vector<uint32_t> &Mesh::getIndices() const
{
  return indices;
}

void Mesh::recenterMesh()
{
  if (vertices.empty()) return;
  glm::vec3 centroid(0.0f);

  for (const auto &v: vertices)
  {
    centroid += v.position;
  }
  centroid /= static_cast<float>(vertices.size());

  for (auto &v: vertices)
  {
    v.position -= centroid;
  }
}