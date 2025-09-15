#include<asset_manager.hpp>

void AssetManager::setTexture(){
    sampler= std::make_unique<Sampler> (device);
    textureCreateInfo  textureInfo; 
    textureInfo.device    =device; 
    textureInfo.sampler   = sampler->get(); 
    textureInfo.filename  = "/home/ljh/CLionProjects/VkMain/extern/externModel/hand/texture/HAND_C.jpg";
    textureInfo.allocator = &allocator;
    texture= std::make_shared<Texture> (textureInfo);
}

void AssetManager::setLight(){ 
    lightManager = std::make_unique<LightManager> (device, allocator);
    Light light ;
    light.position = glm::vec3(1.0,1.0,2.0); 
    light.type = 1;
    light.direction = glm::vec3(0,0,0);
    light.intensity= 0.2; 
    light.color = glm::vec3(1,0.7,0.5);
    lightManager->addLight(light);
    Light light2; 
    light2.position = glm::vec3(1.0,1.0,10.0); 
    light2.type = 1;
    light2.direction = glm::vec3(0,0,0);
    light2.intensity= 0.6; 
    light.color = glm::vec3(0,0.7,0.5);
    lightManager->addLight(light2);
    lightManager->uploadData();
    spdlog::info("add lights");
}

void AssetManager::setCamera(VkExtent2D extent){ 
    camCreateInfo camInfo{}; 
    camInfo.fov         = glm::radians(45.0f); 
    camInfo.aspectRatio = static_cast<float>(extent.width) / static_cast<float>(extent.height);
    camInfo.nearPlane = 0.1f; 
    camInfo.farPlane  = 300.0; 
    camInfo.allocator = &allocator;
    camera= std::make_unique<Camera> (camInfo);
}

void AssetManager::uploadDescriptors(std::array<VkDescriptorSet,3> sets){
    //all layout 0 
    spdlog::info("write descriptor Set");
    camera      ->uploadDescriptor(sets.at(0));
    texture     ->uploadDescriptor(sets.at(1));
    lightManager->uploadDescriptor(sets.at(2));
    spdlog::info("descriptors uploaded ");
}

//mesh and texture buffer -> copy stage to Gpu
void AssetManager::setUp(VkCommandBuffer command){
    importer= new ImporterEx();
    Mesh temp = importer->loadModel("/home/ljh/CLionProjects/VkMain/extern/externModel/hand/hand.fbx", allocator) ;
    delete importer;
    mesh= std::make_unique<Mesh> (temp.getVertices(),temp.getIndices(),allocator);
    mesh->copyBuffer(command);
    texture->copyBufferToImage(command);
}
