#include<sceneRenderer.hpp> 
//void SceneRenderer::init(){}
//set stage
//pipeline : descriptor and pipeline 주업받아서 나오도록 
void SceneRenderer:: createPipeline(std::vector<VkDescriptorSetLayout>* layout_h){
fragShader        = std::make_unique<ShaderModule>     (device_h, fragPath);
vertShader        = std::make_unique<ShaderModule>     (device_h, vertPath);
//create pipeline
//renderPass = 설계도, pipeline = 구현
pipelineCreateInfo info; 
info.device               =   device_h;     
info.extent               =   swapchain->getExtent();     
info.renderPass           =   renderpass_h;
info.vertShaderModule     =   vertShader->get();
info.fragShaderModule     =   fragShader->get(); 
info.descriptorSetLayouts =   layout_h; 
pipeline                  =   std::make_unique<GraphicsPipeline> (info);
pipeline_layout_h         =   pipeline->getLayout();
pipeline_h                =   pipeline->get();
spdlog::info("create pipeline") ; 
}


void SceneRenderer::setUp(VkCommandBuffer command){ 
spdlog::info("draw call");//push semaphore
vkCmdBindPipeline (command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h); // 파이프라인 바인딩
camera->camUpdate(); 
VkViewport        viewport{};
viewport.x        = 0.0f;
viewport.y        = 0.0f;
viewport.width    = (float)swapchain->getExtent().width;
viewport.height   = (float)swapchain->getExtent().height;
viewport.minDepth = 0.0f;
viewport.maxDepth = 1.0f;
vkCmdSetViewport(command, 0, 1, &viewport);
VkRect2D scissor{};
scissor.offset = {0, 0};
scissor.extent = swapchain->getExtent();
vkCmdSetScissor(command, 0, 1, &scissor);
asset.mesh->bind(command);
asset.mesh->draw(command);
}
// 프레임 이전 작업 완료 대기 (CPU와 GPU 간 동기화)
//push semaphore
//  record start 
//  모든 set들을 binding 해줘야함
void SceneRenderer::draw(VkCommandBuffer command){ 
vkCmdBindPipeline (command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h); // 파이프라인 바인딩
camera->camUpdate(); 
VkViewport viewport{};
viewport.x        = 0.0f;
viewport.y        = 0.0f;
viewport.width    = (float)swapchain->getExtent().width;
viewport.height   = (float)swapchain->getExtent().height;
viewport.minDepth = 0.0f;
viewport.maxDepth = 1.0f;
vkCmdSetViewport(command, 0, 1, &viewport);
VkRect2D scissor{};
scissor.offset = {0, 0};
scissor.extent = swapchain->getExtent();
vkCmdSetScissor(command, 0, 1, &scissor);
// must descriptor binding 이후 draw해야만 함 안그러면 터짐
asset.mesh->bind(command);
asset.mesh->draw(command);
}

void SceneRenderer::pushConstant(VkCommandBuffer command, Material material){ 
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(MaterialType);
    //setting materal for rendereing material constant setting
    
    // MaterialType materialData = {};
    // materialData.albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    // materialData.params = glm::vec4(0.5f, 0.3f, 1.0f, 0.0f);  // metallic, roughness, ao, padding
    // materialData.flags = (1 << 0); // 예: useAlbedoMap 플래그

    // vkCmdPushConstants(
    // command,
    // layout,
    // VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
    // 0,
    // sizeof(MaterialType),
    // &materialData
    //);
}




