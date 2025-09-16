#ifndef ASSETMANAGER_HPP
#define ASSETMANAGER_HPP
#include<mesh.hpp>
#include<camera.hpp>
#include <../../source/resource/sampler.hpp>
#include<material.hpp>
#include<importer.hpp>
#include<light.hpp>
#include <memory>

struct assetCreateInfo;
struct Light;
class Mesh;
class TextureManager;
struct Material;
class CameraManager;
class Sampler;
class MemoryAllocator;
class LightManager;
using ResourceKey = std::string;

class resource_manager{
  friend class RenderGraph;
public:
  explicit resource_manager();
  ~resource_manager();
  void init();
  void shutdown();
  bool loadTexture
    (
      const ResourceKey& key,
      const char* path,
      bool srgb    = true,
      bool genMips = true
    );
  std::shared_ptr<TextureManager> getTexture
    (
      const ResourceKey& key
    ) const;
  // Mesh
  bool loadMesh
    (
      const ResourceKey& key,
      const char* path
    );
  std::shared_ptr<Mesh> getMesh
    (
      const ResourceKey& key
    ) const;
  // Material-+++++++++++++++++++++
  bool createMaterial
    (
      const ResourceKey& key,
      const ResourceKey& albedoTexKey /* 필요시 확장: 노말/메탈/러프 등 */
    );
  std::shared_ptr<Material> getMaterial
    (
      const ResourceKey& key
    ) const;

  // Object(간단 파사드: Mesh+Material 조합, 필요시 전용 매니저로 분리)
  bool createObject
    (
      const ResourceKey& key,
      const ResourceKey& meshKey,
      const ResourceKey& matKey
    );

  void createLightManager(); // 호환 유지
  void addLight
    (
      const Light& light
    ); // setLight 대체(의도 반영)
  void setLights
    (
      const std::vector<Light>& lights
    );
  void clearLights();
  void stageUploads(); // CPU측 staging 준비
  void copyToDev
    (
      VkCommandBuffer cmd
    ); // 버퍼/이미지 복사
  void transitionForShaderRead
    (
      VkCommandBuffer cmd
    );                // 레이아웃 전환
  void freeStaging(); // CPU staging 해제
  bool writeDescSets
    (
      std::vector<VkDescriptorSet> sets
    );
  void updatePerFrame(); // 카메라 UBO/라이트 UBO 등
  void applyUiLighting
    (
      const std::vector<Light>& lights
    );
  void requestLoadTextureAsync
    (
      const ResourceKey& key,
      const char* path,
      bool srgb    = true,
      bool genMips = true
    );
  void requestLoadMeshAsync
    (
      const ResourceKey& key,
      const char* path
    );
  void pumpLoads();
  void setUp
    (
      VkCommandBuffer command
    ); // → stageUploads() 등으로 분해 권장
  void copyload
    (
      VkCommandBuffer command
    );              // → copyToDev(cmd)
  void upload();    // → updatePerFrame()
  void stageFree(); // → freeStaging()
private:
  std::shared_ptr<TextureManager> ensureTextureSystem();
  bool writeCameraDesc
    (
      VkDescriptorSet set
    );
  bool writeTextureDesc
    (
      VkDescriptorSet set
    );
  bool writeLightDesc
    (
      VkDescriptorSet set
    );

private:
  ImporterEx importer;
  std::shared_ptr<Sampler> sampler;
  std::unique_ptr<LightManager> lightMgr_;
  std::shared_ptr<CameraManager> camera_; // 기본 카메라
  std::unordered_map<ResourceKey, std::shared_ptr<TextureManager>> textures_;
  std::unordered_map<ResourceKey, std::shared_ptr<Mesh>> meshes_;
  std::unordered_map<ResourceKey, std::shared_ptr<Material>> materials_;
  bool enableBindless_  = false;
  uint32_t maxTextures_ = 0;
};

#endif
