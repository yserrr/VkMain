#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <../geometry/mesh.hpp>
#include <material.hpp>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 선택: std140 호환을 위해 mat3 대신 mat4로 노멀 매트릭스를 보냄
// (std140에서 mat3는 컬럼당 vec4로 패딩되어 48바이트 → mat4(64B)가 실용적)
struct alignas(16) ObjectUBO {
  glm::mat4 model;   // 64B
  glm::mat4 normal;  // 64B (inverse-transpose(model), 상위 3x3 사용)
  glm::uvec4 ids;    // x=objectId, y=materialId, z=meshId, w=flags
  // 총 64+64+16 = 144B (16배수 정렬 OK)
};

static_assert(sizeof(ObjectUBO) % 16 == 0, "ObjectUBO must be 16-byte aligned");

// 레이어/마스크(딥스/섀도/픽커 등 파이프라인 마스킹에 사용)
enum class RenderMask : uint32_t {
  None      = 0x00000000,
  Default   = 0x00000001,
  Shadow    = 0x00000002,
  Picking   = 0x00000004,
  GBuffer   = 0x00000008,
  // 필요 시 확장
};

inline RenderMask operator|(RenderMask a, RenderMask b) {
  return static_cast<RenderMask>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool HasMask(RenderMask m, RenderMask bit) {
  return (static_cast<uint32_t>(m) & static_cast<uint32_t>(bit)) != 0;
}


// 간단 트랜스폼 컴포넌트(TRS)
struct Transform {
  glm::vec3 position{0.0f};
  glm::vec3 scale{1.0f};
  // 회전은 내부적으로 yaw/pitch/roll 또는 쿼터니언으로 구현할 수 있음(여기선 축-각 API만 노출)
  glm::mat4 localMatrix{1.0f};
  bool      dirty = true;
};

using ObjectID   = uint32_t;
using MaterialID = uint32_t;
using MeshID     = uint32_t;

// 퍼-오브젝트 UBO 바인딩 계약(필요하면 엔진 전역 enum과 맞추기)
enum class ObjectSetBinding : uint32_t {
  ObjectUbo = 0 // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER(DYNAMIC) 또는 SSBO
};

class Object {
public:
  // 최소 생성자: 메시/머티리얼만 주입 (기존 인터페이스 호환)
  Object(std::shared_ptr<MeshManager> mesh, std::shared_ptr<Material> material)
  : mesh_(std::move(mesh)), material_(std::move(material)) {
    // 초기 TRS → 로컬 행렬
    rebuildLocalFromTRS();
  }

  // 선택 생성자: IDs/마스크/초기 TRS까지
  Object(std::shared_ptr<MeshManager> mesh, std::shared_ptr<Material> material,
         ObjectID objectId, RenderMask mask = RenderMask::Default)
  : mesh_(std::move(mesh)), material_(std::move(material)), objectId_(objectId), mask_(mask) {
    rebuildLocalFromTRS();
  }

  // -----------------------
  // 트랜스폼: Set 계열
  // -----------------------
  void setPosition (const glm::vec3& pos) {
    tr_.position = pos; tr_.dirty = true; markWorldDirty();
  }
  // angleRadians와 축 기반 회전: 누적 회전으로 합성하려면 rotate() 사용
  void setRotation(float angleRadians, const glm::vec3& axis) {
    // 절대 회전으로 교체가 필요한 경우 제공. 내부는 localMatrix 재구성으로 처리.
    localRotation_ = glm::angleAxis(angleRadians, glm::normalize(axis));
    tr_.dirty = true; markWorldDirty();
  }
  void rotate(float angleRadians, const glm::vec3& axis) {
    // 누적 회전(쿼터니언)
    localRotation_ = glm::normalize(glm::angleAxis(angleRadians, glm::normalize(axis)) * localRotation_);
    tr_.dirty = true; markWorldDirty();
  }
  void setScale(const glm::vec3& s) {
    tr_.scale = s; tr_.dirty = true; markWorldDirty();
  }
  void setTRS(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl) {
    tr_.position = pos; localRotation_ = rot; tr_.scale = scl; tr_.dirty = true; markWorldDirty();
  }
  // 모델 행렬 직접 교체가 필요한 경우(외부 DCC/애니메이션 결과)
  void setModelMatrix(const glm::mat4& m) {
    modelWorld_ = m; worldDirty_ = false; // 직접 설정 시 dirty 해제
    // normalWorld_는 필요 시 update에서 계산
    normalDirty_ = true;
  }

  // -----------------------
  // 계층(부모-자식)
  // -----------------------
  void setParent(Object* p) {
    parent_ = p; markWorldDirty();
  }
  Object* parent() const { return parent_; }

  void addChild(Object* c) {
    if (!c) return;
    children_.push_back(c);
    c->setParent(this);
  }
  const std::vector<Object*>& children() const { return children_; }

  // -----------------------
  // 업데이트/행렬 조회
  // -----------------------
  // 필요 시에만 재계산(더티 플래그 기반)
  void update() const {
    // 로컬 행렬 갱신
    if (tr_.dirty) {
      rebuildLocalFromTRS();
      tr_.dirty = false;
      worldDirty_ = true;
    }
    // 월드 행렬 갱신
    if (worldDirty_) {
      if (parent_) {
        modelWorld_ = parent_->modelWorld_ * tr_.localMatrix;
      } else {
        modelWorld_ = tr_.localMatrix;
      }
      worldDirty_ = false;
      normalDirty_ = true;
      // 자식 전파
      for (auto* ch : children_) if (ch) ch->markWorldDirty();
    }
    // 노멀 행렬 갱신
    if (normalDirty_) {
      // inverse-transpose(modelWorld_) 상위 3x3를 mat4에 담는다(하단 행/열 보정)
      glm::mat4 invT = glm::transpose(glm::inverse(modelWorld_));
      normalWorld_ = invT;
      normalDirty_ = false;
    }
  }

  const glm::mat4& modelMatrix()  const { update(); return modelWorld_; }
  const glm::mat4& normalMatrix() const { update(); return normalWorld_; }

  // -----------------------
  // 리소스/식별/마스크
  // -----------------------
  std::shared_ptr<MeshManager>     mesh()     const { return mesh_; }
  std::shared_ptr<Material> material() const { return material_; }

  void setMesh    (std::shared_ptr<MeshManager> m)     { mesh_ = std::move(m); }
  void setMaterial(std::shared_ptr<Material> m) { material_ = std::move(m); }

  void setObjectId  (ObjectID id)   { objectId_ = id; }
  void setMaterialId(MaterialID id) { materialId_ = id; }
  void setMeshId    (MeshID id)     { meshId_ = id; }
  void setMask(RenderMask m)        { mask_ = m; }
  RenderMask mask() const           { return mask_; }

  // -----------------------
  // 바운딩/컬링
  // -----------------------
  void setLocalBounds(const AABB& b) { localBounds_ = b; boundsDirty_ = true; }
  AABB worldBounds() const {
    update();
    if (boundsDirty_) {
      // 간단한 AABB 변환(정확도 ↑ 필요 시 8코너 변환)
      worldBounds_ = transformAABB(localBounds_, modelWorld_);
      boundsDirty_ = false;
    }
    return worldBounds_;
  }

  // -----------------------
  // UBO/드로우 보조
  // -----------------------
  // 퍼-오브젝트 UBO 채우기(버퍼에 memcpy하기 위한 CPU 구조)
  ObjectUBO buildUBO() const {
    update();
    ObjectUBO u{};
    u.model  = modelWorld_;
    u.normal = normalWorld_;
    u.ids    = glm::uvec4(objectId_, materialId_, meshId_, static_cast<uint32_t>(mask_));
    return u;
  }

  // 실제 드로우: 파이프라인/디스크립터 바인딩은 엔진에서 처리.
  // Object는 안전한 바인딩 순서만 가정하고 메시 드로우만 호출.
  void draw(VkCommandBuffer cmd) const {
    if (!mesh_) return;
    mesh_->bind(cmd);
    mesh_->draw(cmd);
  }

  // 편의 API(인스턴스/서브메시 드로우 등 필요 시 오버로드 추가)
  void draw(VkCommandBuffer cmd, uint32_t submeshIndex) const {
    if (!mesh_) return;
    mesh_->bind(cmd);
    mesh_->draw(cmd, submeshIndex);
  }

  // ---- 기존 호환용 얇은 래퍼 ----
  void setPositionCompat(const glm::vec3& pos) { setPosition(pos); }
  void setRotationCompat(float angleRad, const glm::vec3& axis) { setRotation(angleRad, axis); }
  void setScaleCompat(const glm::vec3& s) { setScale(s); }
  // --------------------------------

private:
  void rebuildLocalFromTRS() const {
    // TRS 합성: T * R * S
    glm::mat4 T = glm::translate(glm::mat4(1.0f), tr_.position);
    glm::mat4 R = glm::mat4_cast(localRotation_);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), tr_.scale);
    tr_.localMatrix = T * R * S;
  }

  void markWorldDirty() {
    worldDirty_ = true; normalDirty_ = true; boundsDirty_ = true;
  }

  static AABB transformAABB(const AABB& b, const glm::mat4& m) {
    // AABB 8코너 변환 후 재계산(안전)
    glm::vec3 corners[8] = {
      {b.min.x, b.min.y, b.min.z}, {b.max.x, b.min.y, b.min.z},
      {b.min.x, b.max.y, b.min.z}, {b.max.x, b.max.y, b.min.z},
      {b.min.x, b.min.y, b.max.z}, {b.max.x, b.min.y, b.max.z},
      {b.min.x, b.max.y, b.max.z}, {b.max.x, b.max.y, b.max.z}
    };
    AABB out;
    for (int i=0;i<8;++i) {
      glm::vec4 p = m * glm::vec4(corners[i], 1.0f);
      out.min = glm::min(out.min, glm::vec3(p));
      out.max = glm::max(out.max, glm::vec3(p));
    }
    return out;
  }

private:
  // 리소스
  std::shared_ptr<MeshManager>     mesh_;
  std::shared_ptr<Material> material_;

  // 식별/마스크
  ObjectID   objectId_   = 0;
  MaterialID materialId_ = 0;
  MeshID     meshId_     = 0;
  RenderMask mask_       = RenderMask::Default;

  // 트랜스폼/계층
  mutable Transform tr_;
  mutable glm::quat localRotation_ = glm::quat(1,0,0,0);
  mutable glm::mat4 modelWorld_{1.0f};
  mutable glm::mat4 normalWorld_{1.0f};
  mutable bool      worldDirty_  = true;
  mutable bool      normalDirty_ = true;

  Object* parent_ = nullptr;
  std::vector<Object*> children_;

  // 바운딩
  AABB       localBounds_{};
  mutable AABB  worldBounds_{};
  mutable bool  boundsDirty_ = true;
};



#endif//