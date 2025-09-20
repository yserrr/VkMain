#include <common.hpp>
#include <swapchain.hpp>
#include <frame_pool.hpp>
#include <command_pool_manager.hpp>
#include <../resource/renderpass_pool.hpp>
#include <../resource/pipeline.hpp>
#include "log_sink.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "resource_manager.hpp"
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

enum class CallType{
  Mesh,
  Texture,
  UPLOAD,
  UPDATE,
  FREE,
};

struct UICall{
  std::string path;
  uint32_t bindingSlot = -1;
  CallType type;
};

class UIRenderer{
  friend class Engine;

public:
  UIRenderer(UIRendererCreateInfo info);

  void rec(VkCommandBuffer command);
  void render(VkCommandBuffer command);
  void draw(VkCommandBuffer command);
  void drawcall(VkCommandBuffer command);
  void drawTransition(VkCommandBuffer command);
  void drawStateWindow(ImVec2 size);
  void drawToolBoxRight(ImVec2 size);
  void drawToolBoxLeft(ImVec2 size);
  void drawToolBoxUnder(ImVec2 size);
  void drawVertexState(ImVec2 size);
  void drawIndexState(ImVec2 size);
  void drawLightState(ImVec2 size);
  void drawTextureState(ImVec2 size);
  void drawCameraState(ImVec2 size);
  void drawMaterialState(ImVec2 size);
  void drawMouseState(ImVec2 size);
  void drawFramebufferState();
  void drawFrame(ImVec2 size);
  void inputText();

  void drawLightUI(ImVec2 size);
  void drawCameraUI();
  void drawMaterialUI();
  void drawShaderUI();
  void setResourceManager(ResourceManager *resourceManager);

private:
  void colorPickerColor();
  void createPool();
  void setupStyle();
  void uploadBackgroundImage();

  bool smallUi_ = true; // 작은 창 유지 토글
  float color[4];
  std::vector<std::string> uploadedMesh_;
  std::vector<std::string> uploadedTexture_;
  std::vector<UICall> callStack_;
  std::shared_ptr<UILogSink> sink_;
  VkDescriptorSet backgroundDescriptor_;
  VulkanTexture *backgroundTexture_;
  ResourceManager *resourceManager_;
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