//
// Created by ljh on 25. 9. 15..
//

#include <ui_renderer.hpp>

void UIRenderer::rec(VkCommandBuffer command)
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void UIRenderer::render(VkCommandBuffer command)
{
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command);
}

void UIRenderer::draw(VkCommandBuffer command)
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  const ImVec2 smallSize = smallUi_ ? ImVec2(360, 200) : ImVec2(440, 220);
  drawFramebufferState();
  drawStateWindow(smallSize);
  drawMouseState(smallSize);
  drawToolBox(smallSize);
//  drawFrame(smallSize);
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command);
}

void UIRenderer::drawcall(VkCommandBuffer command)
{
  const ImVec2 smallSize = smallUi_ ? ImVec2(360, 200) : ImVec2(440, 220);
  drawFramebufferState();
  drawStateWindow(smallSize);
  drawMouseState(smallSize);
  drawToolBox(smallSize);
  drawToolBoxUnder(smallSize);
}

void UIRenderer::drawTransition(VkCommandBuffer command)
{
  ImVec2 size = ImVec2(backgroundTexture_->width / 10, backgroundTexture_->height / 10);
  ImGui::Begin("Image Example");
  ImGui::Image((ImTextureID) (intptr_t) backgroundDescriptor_, size, ImVec2(0, 0), ImVec2(1, 1));
  ImGui::End();
}

void UIRenderer::drawStateWindow(ImVec2 size)
{
  {
    ImVec2 dispSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(dispSize.x - 300, (dispSize.y / 6)), ImGuiCond_Once);
    if (ImGui::Begin("box", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      // drawFramebufferState();
      // drawVertexState(size);
      // drawIndexState(size);
      // drawLightState(size);
      // drawTextureState(size);
      // drawCameraState(size);
      // drawMaterialState(size);
      // drawShaderState(size);
    }
    ImGui::End();
  }
}

void UIRenderer::drawToolBox(ImVec2 size)
{
  {
    ImVec2 dispSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(dispSize.x - 300, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, dispSize.y), ImGuiCond_Once);
    if (ImGui::Begin("Model Tool Box",
                     nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      static char buf[256] = "";
      ImGui::InputTextWithHint("Mesh Load", "Mesh/Load/Path....", buf, IM_ARRAYSIZE(buf));
      static int selected_hint = -1;

      if (ImGui::Button("load Mesh"))
      {
        UICall call;
        call.path = buf;
        call.type = CallType::Mesh;
        callStack_.push_back(call);
        spdlog::info("mesh call {}", call.path);
        buf[0] = '\0';
      }
    }
    {
      static char buf[256] = "";
      ImGui::InputTextWithHint("Texture Load", "texture/Load/Path....", buf, IM_ARRAYSIZE(buf));

      int bindingSlot = 0;
      ImGui::InputInt("binding slot", &bindingSlot);
      if (ImGui::Button("load Texture"))
      {
        UICall call;
        call.path        = buf;
        call.type        = CallType::Texture;
        call.bindingSlot = bindingSlot;
        callStack_.push_back(call);
        spdlog::info("mesh call {}", call.path);
        buf[0] = '\0';
      }
      ImGui::BeginChild("ScrollRegion", ImVec2(0, 300), true);
      {
        if (ImGui::ColorPicker4("Pick Color",
                                color,
                                ImGuiColorEditFlags_PickerHueBar))
        {
          printf("Color changed: R=%.2f G=%.2f B=%.2f A=%.2f\n", color[0], color[1], color[2], color[3]);
        }
      }
      ImGui::EndChild();
    }

    ImGui::End();
  }
}


void UIRenderer::drawToolBoxUnder(ImVec2 size)
{
  ImVec2 dispSize = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(0, (dispSize.y / 6)*5), ImGuiCond_Always);
  ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(dispSize.x - 300, (dispSize.y / 6)), ImGuiCond_Once);
  if (ImGui::Begin("texture", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
  {
    // drawFramebufferState();
    // drawVertexState(size);
    // drawIndexState(size);
    // drawLightState(size);
    // drawTextureState(size);
    // drawCameraState(size);
    // drawMaterialState(size);
    // drawShaderState(size);
  }
  ImGui::End();
}
void UIRenderer::colorPickerColor()
{
  ImGui::Begin("Color Picker");
  if (ImGui::ColorEdit4("Pick Color", color))
  {
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

void UIRenderer::setResourceManager(ResourceManager *resourceManager)
{
  resourceManager_ = resourceManager;
}