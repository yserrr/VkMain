#include<resource_manager.hpp>

void ResourceManager::setTexture() {}
VulkanTexture * ResourceManager::getTexture(Key path)
{
  auto it = textures_.find(path);
  if (it != textures_.end())
    return it->second.get();
  return nullptr;
}

void ResourceManager::setLight() {}

Camera *ResourceManager::getCamera()
{
  return camera.get();
}

void ResourceManager::uploadDescriptors()
{
  spdlog::info("write descriptor Set");
  for (auto &texture: textures_)
  {
    texture.second->uploadDescriptor(bindlessDescirptor_, 0);
  }
  spdlog::info("descriptors uploaded ");
}

//mesh and texture buffer -> copy stage to Gpu
void ResourceManager::loadMesh(VkCommandBuffer command, std::string path)
{
  std::string modelPath = std::string(ASSET_MODELS_DIR) + path;
  spdlog::info("Loading {} ", modelPath.c_str());

  Mesh tempMesh = importer_.loadModel(modelPath.c_str(), allocator_);
  tempMesh.name = path;

  std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(tempMesh.getVertices(), tempMesh.getIndices(), allocator_);
  mesh->copyBuffer(command);
  meshes_[path] = std::move(mesh);
}

void ResourceManager::loadTexture(VkCommandBuffer command, std::string path)
{
  textureCreateInfo textureInfo;
  textureInfo.device      = device_;
  textureInfo.sampler     = samplerBuilder_->get();
  std::string texturePath = std::string(ASSET_TEXTURES_DIR) + path;
  textureInfo.filename    = texturePath.c_str();
  textureInfo.allocator   = &allocator_;
  auto texture            = std::make_unique<VulkanTexture>(textureInfo);
  texture->copyBufferToImage(command);
  texture->uploadDescriptor(bindlessDescirptor_, 0);
  textures_[path] = std::move(texture);
}