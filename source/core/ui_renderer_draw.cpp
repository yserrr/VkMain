//
// Created by ljh on 25. 9. 15..
//

#include <ui_renderer.hpp>

void UIRenderer::draw(VkCommandBuffer command)
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  const ImVec2 smallSize = smallUi_ ? ImVec2(360, 200) : ImVec2(440, 220);
  drawFramebufferState();
  drawStateWindow(smallSize);
  drawMouseState(smallSize);
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command);
}

void UIRenderer::drawStateWindow(ImVec2 size)
{
  {
    ImGui::SetNextWindowPos(ImVec2(20, 0), ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(size, ImGuiCond_Once);
    if (ImGui::Begin("Current State", nullptr, ImGuiWindowFlags_NoMove))
    {
      drawFramebufferState();
      drawVertexState(size);
      drawIndexState(size);
      drawLightState(size);
      drawTextureState(size);
      drawCameraState(size);
      drawMaterialState(size);
      // drawShaderState(size);
    }
    ImGui::End();
  }
}

void UIRenderer::colorPickerColor()
{
  ImGui::Begin("Color Picker");
  if (ImGui::ColorEdit4("Pick Color", color))
  {
    ///todo :
    /// need -> pick color and collback function design
    printf("Color changed: R=%.2f G=%.2f B=%.2f A=%.2f\n", color[0], color[1], color[2], color[3]);
  }
  ImGui::End();
}


/// todo : imple this options 
void UIRenderer::drawLightUI(ImVec2 size)
{
  ImGui::SetNextWindowPos(ImVec2(20, 300), ImGuiCond_Always);
  ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(size, ImGuiCond_Once);
  if (ImGui::Begin("Light setting", nullptr, ImGuiWindowFlags_NoMove))
  {
    //cpu::light light{};
    ImGui::Button("light : Position ");
    ImGui::Button("light : Direction");
    ImGui::Button("light : Color ");
  }
  ImGui::End();
}

void UIRenderer::drawCameraUI()
{
  if (ImGui::Begin("Light setting", nullptr, ImGuiWindowFlags_NoMove))
  {
    ImGui::Button("light : Position ");
    ImGui::Button("light : Direction");
    ImGui::Button("light : Color ");
  }
  ImGui::End();
}

void UIRenderer::drawMaterialUI()
{
  if (ImGui::Begin("Light setting", nullptr, ImGuiWindowFlags_NoMove))
  {
    ImGui::Button("light : Position ");
    ImGui::Button("light : Direction");
    ImGui::Button("light : Color ");
  }
  ImGui::End();
}

void UIRenderer::drawShaderUI()
{
  if (ImGui::Begin("Light setting", nullptr, ImGuiWindowFlags_NoMove))
  {
    ImGui::Button("light : Position ");
    ImGui::Button("light : Direction");
    ImGui::Button("light : Color ");
  }
  ImGui::End();
}