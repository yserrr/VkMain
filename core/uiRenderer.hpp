#include <common.hpp> 
#include <swapChain.hpp> 
#include <framebuffer.hpp> 
#include <commandBuffer.hpp>
#include <renderPass.hpp>
#include <pipeline.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <logicalDevice.hpp> 
#ifndef UIRENDERER_HPP
#define UIRENDERER_HPP

struct UIRendererCreateInfo{ 
GLFWwindow*            window_h; 
VkInstance             instance_h;
VkDevice               device_h;
VkPhysicalDevice       physical_device_h;
VkRenderPass           renderpass_h;
uint32_t               graphics_family; 
uint32_t               present_family;
VkQueue                graphics_q;
};

class UIRenderer{ 
public: 
UIRenderer(UIRendererCreateInfo info);
void draw(VkCommandBuffer command);
private:
void createPool();
uint32_t graphics_family ; 
uint32_t present_family  ;

VkQueue                graphics_q;
GLFWwindow*            window_h;
VkDevice               device_h;
VkPhysicalDevice       physical_device_h; 
VkInstance             instance_h;
VkRenderPass           renderpass_h;
VkDescriptorPool       imguiPool;
};


#endif 

















