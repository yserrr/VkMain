// Mesh.hpp
#ifndef MESH_HPP
#define MESH_HPP

#include <vertex.hpp>
#include <buffer.hpp>

class Mesh {
public:
// move constructor
Mesh(const std::vector<Vertex>&     vertices, 
     const std::vector<uint32_t>&   indices,
     MemoryAllocator&             allocator)
    : vertices(vertices), indices(indices), allocator(allocator)
{
recenterMesh();
vertexSize   = (sizeof(vertices[0]))* vertices.size();
indiceSize   = sizeof(indices[0]) * indices.size();
vertexBuffer = std::make_unique<Buffer>(allocator,vertexSize, BufferType::Vertex);
indexBuffer  = std::make_unique<Buffer>(allocator,indiceSize, BufferType::Index);
vertexBuffer->getStagingBuffer(vertices.data());
indexBuffer ->getStagingBuffer(indices.data());
vertexBuffer->createMainBuffer(); 
indexBuffer ->createMainBuffer();
}

void copyBuffer(VkCommandBuffer commandBuffer ) const { 
vertexBuffer->copyBuffer(commandBuffer);
indexBuffer ->copyBuffer(commandBuffer);
}
void bind(VkCommandBuffer commandBuffer){
VkDeviceSize offsets[] = {0};
VkBuffer vertexBufs[] = { *(vertexBuffer->getBuffer()) };
vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBufs, offsets);
vkCmdBindIndexBuffer(commandBuffer, *(indexBuffer->getBuffer()), 0, VK_INDEX_TYPE_UINT32);
}

void draw (VkCommandBuffer commandBuffer) const {
vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
//spdlog::info("draw call");
}

~Mesh(){
}
const std::vector<Vertex>&  getVertices() const { return vertices; }
const std::vector<uint32_t>& getIndices() const { return indices;  }
 
void recenterMesh() {
    if (vertices.empty()) return;
    glm::vec3 centroid(0.0f);

    // 1. 정점 위치 평균을 구해서 중심점을 계산
    for (const auto& v : vertices) {
        centroid += v.position;
    }
    centroid /= static_cast<float>(vertices.size());

    // 2. 모든 정점을 중심 기준으로 이동
    for (auto& v : vertices) {
        v.position -= centroid;
    }
}

private:
std::unique_ptr<Buffer> vertexBuffer; 
std::unique_ptr<Buffer> indexBuffer;

std::vector<Vertex>     vertices;
std::vector<uint32_t>   indices;

MemoryAllocator &allocator;
VkDeviceSize     vertexSize; 
VkDeviceSize     indiceSize;


};
#endif //MESH_HPP