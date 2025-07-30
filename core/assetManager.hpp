#include <texture.hpp>
#include <mesh.hpp>
//#include <material.hpp> 
#include <light.hpp> 
#include <logicalDevice.hpp>
#include <common.hpp> 
#include <descriptorPoolManager.hpp>
#include <importer.hpp>
#include <material.hpp>
#include <camera.hpp> 

#ifndef ASSETMANAGER_HPP
#define ASSETMANAGER_HPP 

struct assetCreateInfo{ 
VkDevice           device; 
MemoryAllocator*   allocator;
};

//material class setup 하기
class AssetManager{
public:
AssetManager(const assetCreateInfo &info): 
device       ( info.device), 
allocator    (*info.allocator) 

{} 
~AssetManager(){}

Mesh loadMesh();
void uploadDescriptors(std::array<VkDescriptorSet,3> sets );
void setUp            (VkCommandBuffer command); 
void setCamera        (VkExtent2D extent);
void setTexture(); 
void setLight();

Camera* getCamera() { return camera.get();}
std::unique_ptr<Mesh>                      mesh;
//mesh.setUp(command);

Importer*                             importer;
private:
VkDevice                              device;
MemoryAllocator&                      allocator;
std::unique_ptr<Sampler>              sampler;
std::shared_ptr<Texture>              texture;
std::shared_ptr<Texture>              nomal;

std::unique_ptr<LightManager>         lightManager;
std::unique_ptr<Camera>               camera;
std::array<VkDescriptorSet,3>         descriptors;
};


#endif