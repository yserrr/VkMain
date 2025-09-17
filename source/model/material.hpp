#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#define MATERIAL_MAX 32
#include <material_cfg.hpp>

class TextureManager;
enum class MaterialMap:uint32_t;
struct MatMeta;
using MAT_ID = std::string;


class Material{
  friend class UIControler;
public:
  Material() = default;
  void setDesc(const MaterialDesc &desc);
  void setBaseColor(const glm::vec4 &color);
  void setBaseColor(float r, float g, float b, float a = 1.0f);
  void setMetallic(float v);
  void setRoughness(float v);
  void setAo(float v);
  void setEmission(float s);
  void setEmissionColor(const glm::vec3 &c);
  void setNormalScale(float s);
  void setAlphaCutoff(float v);
  void setDoubleSided(bool b);
  void setAlphaBlend(bool b);

  float getMetallic() const;
  float getRoughness() const;
  uint32_t getAlbedoTexture() const;
  uint32_t getNormalTexture() const;
  uint32_t getMetallicTexture() const;
  glm::vec3 getBaseColor() const;

  uint32_t getMaterialIndex(MAT_ID id) const;
private:
  //infunction

private:
  std::unordered_map<MAT_ID, uint32_t> settedMat_;
};

//void setMapUV(MaterialMap map, float tilingU, float tilingV, float offsetU, float offsetV);
//void setMapStrength(MaterialMap map, float s);
#endif