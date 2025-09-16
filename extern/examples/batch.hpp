#ifndef RENDER_BATCH_HPP
#define RENDER_BATCH_HPP
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include<material.hpp>
#include <buffer.hpp>
struct MaterialUBO;
using MAP_ID = std::string;

class MaterialUploader{
public:
  MaterialUploader();
  void uploadMaterial();
  std::vector<uint32_t> gpuIndices_;
  void setLights(const std::vector<Material> &lights);
  //  bool     updateLight(uint32_t index, const LightCPU& l);
  std::unique_ptr<MaterialBuffer> materialUbo_; // LightUBO 저장
  void uploadDescriptor(VkDescriptorSet set, // 디스크립터 셋에 UBO/IBL/Shadow 바인드
                        uint32_t bindingUbo = (uint32_t) LightSetBinding::LightUbo,
                        uint32_t bindingIrr = (uint32_t) LightSetBinding::IblIrradiance,
                        uint32_t bindingPref = (uint32_t) LightSetBinding::IblPrefiltered,
                        uint32_t bindingBrdf = (uint32_t) LightSetBinding::IblBrdfLut,
                        uint32_t bindingSh = (uint32_t) LightSetBinding::ShadowMaps);
  void rebuildUBO();
};
using MeshID = uint32_t;
using MaterialID = uint32_t;
using PipelineID = uint32_t; // 혹은 VkPipeline (캐싱되어 사용된다고 가정)

// ------------ 실제 메쉬 데이터를 참조하는 구조 (RenderBatch 내부에서 참조될 것임) ------------
struct MeshData{
  VkBuffer vertexBuffer;
  VkDeviceSize vertexBufferSize; // 전체 버텍스 버퍼 사이즈
  VkBuffer indexBuffer;
  VkDeviceSize indexBufferSize; // 전체 인덱스 버퍼 사이즈
  VkIndexType indexType; // VK_INDEX_TYPE_UINT16 or VK_INDEX_TYPE_UINT32
};

// SubMesh는 MeshData의 특정 부분을 Material과 연결하는 논리적 정보
// 이는 Model Asset에 종속되어 로드될 것임
struct SubMeshInfo{
  uint32_t firstIndex; // MeshData의 인덱스 버퍼 내에서 이 SubMesh의 시작 인덱스
  uint32_t indexCount; // 이 SubMesh가 사용하는 인덱스의 개수
  uint32_t vertexOffset; // MeshData의 버텍스 버퍼 내에서 이 SubMesh의 시작 버텍스 오프셋
  MaterialID materialID; // 이 SubMesh에 적용될 Material의 ID
};

/**
 * @brief RenderBatch는 렌더링을 위해 필요한 모든 정보를 캡슐화합니다.
 *        주로 동일한 렌더링 파이프라인과 Descriptor Set을 공유하는
 *        하나 이상의 SubMesh를 그리기 위한 데이터를 담습니다.
 *        이는 렌더러의 렌더 큐에 쌓이거나, 렌더 그래프의 노드에 전달될 수 있습니다.
 */
struct RenderBatch{
  // 1. 사용할 렌더링 파이프라인 (셰이더, 렌더 스테이트 등)
  // 실제 VkPipeline을 직접 가질 수도 있고, PipelineManager가 관리하는 ID일 수도 있음.
  PipelineID pipelineId;
  VkPipelineLayout pipelineLayout; // 파이프라인 레이아웃 (Descriptor Set Layout 참조용)

  // 2. 이 Batch에서 바인딩될 Descriptor Set들 (주로 전역 UBO, 씬 정보, 조명 등)
  // Material Descriptor Set은 MaterialInfo 내에 있을 수 있음.
  std::vector<VkDescriptorSet> globalDescriptorSets;
  // 각 Set이 어떤 Layout을 가지는지 (e.g., set 0 for camera, set 1 for light)
  // 이는 pipelineLayout에 포함되므로, 실제 드로우 시 파이프라인 바인딩으로 해결됨.
  // ---------- 메쉬 데이터 참조 (이 Batch가 그릴 대상의 지오메트리) ----------
  // 이 Batch가 참조하는 실제 GPU 버퍼 데이터를 가진 Mesh
  // MeshManager나 AssetManager가 관리하는 MeshData의 포인터나 ID
  // (보통 하나의 MeshData 객체만 바인딩 후 여러 SubMeshInfo를 그린다)
  const MeshData *meshData;
  // ---------- 그릴 SubMesh 정보 및 Material 참조 ----------
  // 이 Batch에 포함된 SubMesh와 각 SubMesh에 적용될 Material 정보
  // 보통 이 SubMeshInfo 내의 materialID를 이용해 실제 Material의 DescriptorSet을 바인딩함
  SubMeshInfo subMesh;
  // SubMeshInfo에 MaterialID가 있으므로, 여기서는 해당 Material이 가진 VkDescriptorSet을 직접 참조.
  // 이는 MaterialManager 등에서 MaterialID를 이용해 조회될 수 있음.
  VkDescriptorSet materialDescriptorSet; // 이 SubMesh의 Material Descriptor Set

  // ---------- 오브젝트별 데이터 (Instance Data 또는 Per-Draw Uniforms) ----------
  // Batch에 속한 오브젝트들의 Model Matrix 및 기타 인스턴스별 데이터
  // Instancing을 사용하는 경우, 이 데이터들이 하나의 인스턴스 버퍼에 모여있을 수 있음
  // 또는 매 Draw Call 마다 UBO를 업데이트할 수도 있음.
  std::vector<glm::mat4> modelMatrices; // 배치 내 각 오브젝트의 월드 변환 행렬

  // 만약 Instancing을 사용한다면, 아래 정보가 필요할 수 있음
  VkBuffer instanceBuffer; // 인스턴스 데이터가 담긴 버퍼 (선택 사항)
  uint32_t instanceOffset; // 인스턴스 버퍼 내 오프셋
  uint32_t instanceCount; // 그릴 인스턴스의 개수 (== modelMatrices.size())
  // 만약 인스턴스별 추가 데이터(예: 색상)가 있다면 여기에 필드를 추가할 수 있음.

  // ---------- 기타 드로우 플래그 / 메타데이터 ----------
  uint32_t drawCallCount;
  // 이 Batch에 의해 실제로 몇 번의 Draw Call이 일어날지 예상 (e.g. 1 for non-instanced, instanceCount for instanced)
  bool isInstanced; // 이 Batch가 인스턴싱 드로우인지 여부

  // 디버깅/프로파일링용 이름
  std::string debugName;

  // TODO: Material Proties를 직접 바인딩해야 할 경우, 여기 MaterialID 대신
  // VkDescriptorSet* pMaterialDescriptorSet 이나 MaterialBufferData 같은 것을 넣을 수 있음.
  // 이는 MaterialManager가 MaterialID로부터 실제 DescriptorSet을 검색해서 넣어줄 것임.
  RenderBatch()
  : pipelineId(0), pipelineLayout(VK_NULL_HANDLE), meshData(nullptr),
    instanceBuffer(VK_NULL_HANDLE), instanceOffset(0), instanceCount(0),
    drawCallCount(0), isInstanced(false) {}

  // 필요하다면, 이 Batch를 채우는 헬퍼 함수들을 여기에 추가할 수 있음.
};

#endif // RENDER_BATCH_HPP