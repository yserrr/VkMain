//using stb_image
#define   STB_IMAGE_IMPLEMENTATION 
#include <Engine.hpp>

void Engine::mainLoop(){ 
    setUp();
    spdlog::info("render set up");
    setUp();
    while(!glfwWindowShouldClose(window->get())){
        glfwPollEvents();    
        inFlightFences->  wait(currentFrame); 
        inFlightFences-> reset(currentFrame);
        //push semaphore
        VkSemaphore semaphore       = imageAvailableSemaphores->get(currentFrame);
        imageIndex                  = swapchain->getImageIndex(semaphore);
        VkCommandBuffer command     = rec(imageIndex);
        vkCmdBindDescriptorSets   (command,VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_h,  0,3, sets.data(), 0, nullptr);
        vkCmdBeginRenderPass      (command, &renderPassInfos[imageIndex], VK_SUBPASS_CONTENTS_INLINE); // RenderPass 시작
        sceneRenderer->draw(command); 
        vkCmdNextSubpass(command,VK_SUBPASS_CONTENTS_INLINE);         
        uiRenderer->draw(command);
        vkCmdEndRenderPass(command);
        summitQueue(command,imageIndex);
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    vkDeviceWaitIdle(device_h);  // GPU가 다 끝났다는 보장
    inFlightFences.reset(); 
    renderFinishedSemaphores.reset(); 
    imageAvailableSemaphores.reset();
    sceneRenderer.reset();
    asset.reset();
    allocator.reset();
    interaction.reset(); 
    uiRenderer.reset();
    swapchain.reset();
    imageManager.reset();
    renderPass.reset();
    descriptorManager.reset();
    frameBufferManager.reset();
    commandBufferManager.reset();
    commandPool.reset();
    device.reset();
    physicalDevice.reset();
    surface.reset();
    instance.reset();
    window.reset();
}

void Engine::setUp(){
    inFlightFences->  wait(currentFrame); 
    inFlightFences-> reset(currentFrame);
    VkSemaphore     semaphore   = imageAvailableSemaphores->get(currentFrame);
    imageIndex                  = swapchain->getImageIndex(semaphore);
    VkCommandBuffer command     = rec(imageIndex);
    vkCmdBindDescriptorSets   (command,VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_h,  0,3, sets.data(), 0, nullptr);
    asset->setUp(command);
    vkCmdBeginRenderPass  (command, &renderPassInfos[imageIndex], VK_SUBPASS_CONTENTS_INLINE); // RenderPass 시작
    sceneRenderer->setUp(command);
    vkCmdNextSubpass(command,VK_SUBPASS_CONTENTS_INLINE);          
    uiRenderer   ->draw(command);
    vkCmdEndRenderPass(command);
    summitQueue(command,imageIndex);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VkCommandBuffer Engine::rec(uint32_t imageIndex){
clearValues[0].color                          = {0.0f,0.0f,0.0f,1.0f};
clearValues[1].depthStencil                   = {1.0f, 0}; // 깊이 초기화 값
renderPassInfos[imageIndex].sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
renderPassInfos[imageIndex].renderPass        = renderPass->get();
renderPassInfos[imageIndex].framebuffer       = frameBufferManager->get(imageIndex);
renderPassInfos[imageIndex].renderArea.offset = {0, 0};
renderPassInfos[imageIndex].renderArea.extent = swapchain->getExtent();
renderPassInfos[imageIndex].clearValueCount   = 2;
renderPassInfos[imageIndex].pClearValues      = clearValues.data();
VkCommandBuffer  command = commandBufferManager->record(imageIndex);
return command;
}


void Engine::summitQueue(VkCommandBuffer command, uint32_t imageIndex){
commandBufferManager->endRecord(imageIndex);
VkSubmitInfo submitInfo{};
submitInfo.sType                           = VK_STRUCTURE_TYPE_SUBMIT_INFO;
VkSemaphore          waitSemaphores[]      = {imageAvailableSemaphores->get(currentFrame)};
VkPipelineStageFlags waitStages[]          = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
submitInfo.waitSemaphoreCount              = 1;
submitInfo.pWaitSemaphores                 = waitSemaphores;
submitInfo.pWaitDstStageMask               = waitStages;
submitInfo.commandBufferCount              = 1;
submitInfo.pCommandBuffers                 = &command;
//end signal setting
VkSemaphore signalSemaphores[]    = { renderFinishedSemaphores->get(currentFrame) };
submitInfo.signalSemaphoreCount   = 1;
submitInfo.pSignalSemaphores      = signalSemaphores;
if (vkQueueSubmit(graphics_q, 1, &submitInfo, inFlightFences->get(currentFrame)) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
    }
VkSwapchainKHR swapchains[] = { swapchain->get() };
// 프레젠트 정보 설정
VkPresentInfoKHR presentInfo{};
presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
presentInfo.waitSemaphoreCount = 1;
presentInfo.pWaitSemaphores    = signalSemaphores;
presentInfo.swapchainCount     = 1;
presentInfo.pSwapchains        = swapchains;
presentInfo.pImageIndices      = &(imageIndex);
if(vkQueuePresentKHR(present_q, &presentInfo)!= VK_SUCCESS){
    throw std::runtime_error("present Queue error");
    }
}


//꼭 필요한 것 
