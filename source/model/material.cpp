#include <importer_desc.hpp.hpp>
#include <texture.hpp>
#include <material.hpp>
#include <glm/glm.hpp>

void Material::setDesc(const MaterialDesc &desc){
  setBaseColor(desc.baseColor);
  setMetallic(desc.params.x);
  setRoughness(desc.params.y);
  setAo(desc.params.z);
  setEmission(desc.params.w);
  uint32_t flag = desc.flags;
  //todo: update flags and option
  if (flag & DescriptorBindFlags::ImageBaseColor) index_.albedoIndex = desc.texAlbedo;
  if (flag & DescriptorBindFlags::ImageNormalMap) index_.albedoIndex = desc.texNormal;
  if (flag & DescriptorBindFlags::ImageMetallicMap) index_.metallicTexIndex = desc.texMetallic;
  if (flag & DescriptorBindFlags::ImageRoughnessMap) index_.roughnessTexIndex = desc.texRoughness;
  if (flag & DescriptorBindFlags::ImageAoMap) index_.aoTexIndex = desc.texAo;
  if (flag & DescriptorBindFlags::ImageEmissionMap) index_.emissionTexIndex = desc.texEmissive;
}

void Material::setBaseColor(const glm::vec4 &color)
{
  params_.baseColor = color;
}

void Material::setBaseColor(float r, float g, float b, float a)
{
  params_.baseColor = glm::vec4(r, g, b, a);
}

void Material::setMetallic(float v)
{
  params_.metallic = glm::clamp(v, 0.0f, 1.0f);
}

void Material::setRoughness(float v)
{
  params_.roughness = glm::clamp(v, 0.0f, 1.0f);
}

void Material::setAo(float v)
{
  params_.ao = glm::clamp(v, 0.0f, 1.0f);
}

void Material::setEmission(float s)
{
  params_.emission = std::max(0.0f, s);
}

void Material::setEmissionColor(const glm::vec3 &c)
{
  params_.emissiveColor = c;
}

void Material::setNormalScale(float s)
{
  params_.normalScale = s;
}

void Material::setAlphaCutoff(float v)
{
  params_.alphaCutoff = glm::clamp(v, 0.0f, 1.0f);
}

void Material::setDoubleSided(bool b)
{
  params_.doubleSided = b;
}

void Material::setAlphaBlend(bool b)
{
  params_.alphaBlend = b;
}

float Material::getMetallic() const
{
  return params_.metallic;
}

float Material::getRoughness() const
{
  return params_.roughness;
}

uint32_t Material::getAlbedoTexture() const
{
  return index_.albedoIndex;
}

uint32_t Material::getNormalTexture() const
{
  return index_.normalIndex;
}

uint32_t Material::getMetallicTexture() const
{
  return index_.metallicTexIndex;
}

glm::vec3 Material::getBaseColor() const
{
  return params_.baseColor;
}

