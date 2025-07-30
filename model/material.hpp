#include <vulkan/vulkan.h>
#include <string>
#include <memory>
#include <texture.hpp>

#define MAX_MATERIALS 4
#ifndef MATERIAL_HPP
#define MATERIAL_HPP


struct MaterialType {
    glm::vec4 albedo;      // 16 bytes (offset 0)
    glm::vec4 params;      // metallic, roughness, ao, padding (16 bytes, offset 16)
    uint32_t flags;         // 4 bytes (offset 32) //flag for material type
    uint32_t albedoIndex;
    uint32_t normalIndex;
    uint32_t metallicTexIndex;
    uint32_t roughnessTexIndex;
    uint32_t aoTexIndex;
    uint32_t emissionTexIndex;
    uint32_t padding;
}; 



class Material {
public:
Material(const std::string& name)
: name(name),
   baseColor{1.0f, 1.0f, 1.0f, 1.0f},
   metallic(0.0f),
  roughness(1.0f)
{}

// 기본 색상 설정
void setBaseColor(float r, float g, float b, float a = 1.0f) {
    baseColor[0] = r;
    baseColor[1] = g;
    baseColor[2] = b;
    baseColor[3] = a;
}

// 텍스처 세터
void setAlbedoTexture(std::shared_ptr<Texture> tex) { albedoTexture = tex; }
void setNormalTexture(std::shared_ptr<Texture> tex) { normalTexture = tex; }
void setMetallicRoughnessTexture(std::shared_ptr<Texture> tex) { metallicRoughnessTexture = tex; }
// 셰이더 등에서 쓸 수 있게 게터 제공
const float* getBaseColor()                            const { return baseColor; }
      float  getMetallic()                             const { return metallic;  }
      float  getRoughness()                            const { return roughness; }
std::shared_ptr<Texture> getAlbedoTexture()            const { return albedoTexture; }
std::shared_ptr<Texture> getNormalTexture()            const { return normalTexture; }
std::shared_ptr<Texture> getMetallicRoughnessTexture() const { return metallicRoughnessTexture; }
private:
std::string name;
float baseColor[4];   // RGBA 기본색상
float metallic;       // 금속도
float roughness;      // 거칠기
std::shared_ptr<Texture> albedoTexture;
std::shared_ptr<Texture> normalTexture;
std::shared_ptr<Texture> metallicRoughnessTexture;
};

#endif 