#include "dyn_mesh.hpp"


DynMesh::~DynMesh() {}
DynMesh::DynMesh(const std::vector<VertexAll> &vertices,
                 const std::vector<uint32_t> &indices,
                 MemoryAllocator &allocator) : vertices(vertices), indices(indices), allocator(allocator)
{
  recenterMesh();
  reNomalCompute();
  vertexSize   = (sizeof(vertices[0])) * vertices.size();
  vertexSize= vertexSize* 4;
  indiceSize   = sizeof(indices[0]) * indices.size();
  indiceSize= indiceSize* 4;
  vertexBuffer = std::make_unique<StaticBuffer>(allocator, vertexSize, BufferType::VERTEX);
  indexBuffer  = std::make_unique<StaticBuffer>(allocator, indiceSize, BufferType::INDEX);
  vertexBuffer->getStagingBuffer(vertices.data());
  indexBuffer->getStagingBuffer(indices.data());
  vertexBuffer->createMainBuffer();
  indexBuffer->createMainBuffer();
}

void DynMesh::copyBuffer(VkCommandBuffer commandBuffer) const
{
  vertexBuffer->copyBuffer(commandBuffer);
  indexBuffer->copyBuffer(commandBuffer);
}

void DynMesh::bind(VkCommandBuffer commandBuffer)
{
  VkDeviceSize offsets[] = {0};
  VkBuffer vertexBufs[]  = {*(vertexBuffer->getBuffer())};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBufs, offsets);
  vkCmdBindIndexBuffer(commandBuffer, *(indexBuffer->getBuffer()), 0, VK_INDEX_TYPE_UINT32);
}

void DynMesh::draw(VkCommandBuffer commandBuffer) const
{
  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void DynMesh::dynMeshUpdate(VkCommandBuffer commandBuffer)
{
  vertexBuffer->getStagingBuffer(vertices.data());
  indexBuffer->getStagingBuffer(indices.data());
  vertexBuffer->copyBuffer(commandBuffer);
  indexBuffer->copyBuffer(commandBuffer);
}

const std::vector<VertexAll> &DynMesh::getVertices() const
{
  return vertices;
}

const std::vector<uint32_t> &DynMesh::getIndices() const
{
  return indices;
}

void DynMesh::recenterMesh()
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

void DynMesh::reNomalCompute()
{
  for (auto &v: vertices)
  {
    v.normal = glm::vec3(0.0f);
  }
  for (size_t i = 0; i < indices.size(); i += 3)
  {
    VertexAll &v0 = vertices[indices[i]];
    VertexAll &v1 = vertices[indices[i + 1]];
    VertexAll &v2 = vertices[indices[i + 2]];

    glm::vec3 normal = glm::normalize(glm::cross(v1.position - v0.position, v2.position - v0.position));
    v0.normal += normal;
    v1.normal += normal;
    v2.normal += normal;
  }
  for (auto &v: vertices)
  {
    v.normal = glm::normalize(v.normal);
  }
}