#include <common.hpp>
#include <swapChain.hpp>
#include <frame_pool.hpp>
#include <command_pool_manager.hpp>
#include <renderpass.hpp>
#include <pipeline.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#ifndef UIRENDERER_HPP
#define UIRENDERER_HPP

struct UIRendererCreateInfo{
  GLFWwindow *window_h;
  VkInstance instance_h;
  VkDevice device_h;
  VkPhysicalDevice physical_device_h;
  VkRenderPass renderpass_h;
  uint32_t graphics_family;
  uint32_t present_family;
  VkQueue graphics_q;
  bool useDynamic = false;
};

class UIRenderer{
public:
  UIRenderer(UIRendererCreateInfo info);
  void draw(VkCommandBuffer command);
  void drawStateWindow(ImVec2 size);
  void drawVertexState(ImVec2 size);
  void drawIndexState(ImVec2 size);
  void drawLightState(ImVec2 size);
  void drawTextureState(ImVec2 size);
  void drawCameraState(ImVec2 size);
  void drawMaterialState(ImVec2 size);
  void drawMouseState(ImVec2 size);
  void drawFramebufferState();

  void drawLightUI(ImVec2 size);
  void drawCameraUI();
  void drawMaterialUI();
  void drawShaderUI();

private:
  void colorPickerColor();
  void createPool();
  void setupStyle();

  bool smallUi_ = true; // 작은 창 유지 토글
  float color[4];

  uint32_t graphics_family;
  uint32_t present_family;
  VkQueue graphics_q;
  GLFWwindow *window_h;
  VkDevice device_h;
  VkPhysicalDevice physical_device_h;
  VkInstance instance_h;
  VkRenderPass renderpass_h;
  VkDescriptorPool imguiPool;
};

#endif