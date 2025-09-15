#include<ui_renderer.hpp>

#include "ui_renderer.hpp"
#include<fstream>
#include <filesystem>
/// todo:
///  vertex and index :
///  check height and index size current state draw call is need

void UIRenderer::drawVertexState(ImVec2 size)
{
  {
    ImGui::SetNextWindowPos(ImVec2(20, 55), ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(size);
    if (ImGui::Begin("Vertex State:", nullptr, ImGuiWindowFlags_NoMove))
    {
      ImGui::BeginChild("ScrollRegion", ImVec2(0, 300), true);
      //auto v_state = controler.checkVertexState();

      ImGui::Text("Vertices:");
      //for (int i = 0; i < v_state.size i++)
      //{
      //  ImGui::Text(" [%d] : %f, %f, %f", i, v_state[i].position.x, v_state[i].position.y, v_state[i].position.z);
      //}
      //ImGui::EndChild();
    }
    ImGui::End();
  }
}

void UIRenderer::drawIndexState(ImVec2 size)
{
  {
    ImGui::SetNextWindowPos(ImVec2(20, 80), ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(size);
    if (ImGui::Begin("Index State:", nullptr, ImGuiWindowFlags_NoMove))
    {
      ImGui::BeginChild("ScrollRegion", ImVec2(0, 300), true); // 높이 300
      ImGui::Text("Indices:");
      //auto i_state = controler.checkIndexState();
      //for (int i = 0; i < i_state.size i++)
      //{
      //  ImGui::Text(" [%d] : %d", i, i_state[i]);
      //}
      ImGui::EndChild();
    }
    ImGui::End();
  }
}

void UIRenderer::drawLightState(ImVec2 size)
{
  {
    ImGui::SetNextWindowPos(ImVec2(20, 105), ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(size);
    if (ImGui::Begin("Light State:", nullptr, ImGuiWindowFlags_NoMove))
    {
      ImGui::BeginChild("ScrollRegion", ImVec2(0, 300), true); // 높이 300
      ImGui::Text("current light state :");
      //std::vector<cpu::light> light_state = controler.checkLightState();
      //for (int i = 0; i < light_state.size i++)
      //{
      //  ImGui::Text(" light[%d] position:  %f, %f, %f",
      //              i,
      //              light_state[i].position.x,
      //              light_state[i].position.y,
      //              light_state[i].position.z);
//
      //  ImGui::Text(" light[%d] color:  %f, %f, %f",
      //              i,
      //              light_state[i].color.r,
      //              light_state[i].color.g,
      //              light_state[i].color.b);
      //  if (light_state[i].type == LightType::POINT)
      //  {
      //    ImGui::Text(" light[%d]: light type : POINT", i);
      //  }
      //  if (light_state[i].type == LightType::DIRECTIONAL)
      //  {
      //    ImGui::Text(" light[%d]: light type : DIRENCTION", i);
      //    ImGui::Text(" light[%d] direction:  %f, %f, %f",
      //                i,
      //                light_state[i].direction.x,
      //                light_state[i].direction.y,
      //                light_state[i].direction.z);
      //  }
      //  if (light_state[i].type == LightType::SPOT)
      //  {
      //    ImGui::Text(" light[%d]: light type : SPOT", i);
      //  }
      //  ImGui::Text(" light[%d] range:  %f,", i, light_state[i].range);
      //  ImGui::Text(" light[%d] inner degree:  %f,", i, light_state[i].innerDeg);
      //  ImGui::Text(" light[%d] intensity:  %f,", i, light_state[i].intensity);
      ImGui::Separator();
    }
    ImGui::EndChild();
  }
  ImGui::End();
}

void UIRenderer::drawTextureState(ImVec2 size)
{
  {
    ImGui::SetNextWindowPos(ImVec2(20, 130), ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(size);
    if (ImGui::Begin("texture State:", nullptr, ImGuiWindowFlags_NoMove))
    {
      ImGui::BeginChild("ScrollRegion", ImVec2(0, 300), true); // 높이 300
      ImGui::Text("light :");
      //auto i_state = controler.checkLightState();
      // for (int i = 0; i < i_state.size i++)
      // {
      //   ImGui::Text(" [] light state :");
      // }
      ImGui::EndChild();
    }
    ImGui::End();
  }
}

void UIRenderer::drawMaterialState(ImVec2 size)
{
  {
    ImGui::SetNextWindowPos(ImVec2(20, 155), ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(size);
    if (ImGui::Begin("Material State:", nullptr, ImGuiWindowFlags_NoMove))
    {
      ImGui::BeginChild("ScrollRegion", ImVec2(0, 300), true); // 높이 300
      ImGui::Text("Material:: need to set up material structure");
      ///todo :
      /// set upm maaterial -> hot load text
      ImGui::EndChild();
    }
    ImGui::End();
  }
}

void UIRenderer::drawCameraState(ImVec2 size)
{
  {
    ImGui::SetNextWindowPos(ImVec2(20, 180), ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(size);
    if (ImGui::Begin("Camera State:", nullptr, ImGuiWindowFlags_NoMove))
    {
      //auto temp = controler.checkCameraState();
      ImGui::BeginChild("ScrollRegion", ImVec2(0, 300), true); // 높이 300
      ImGui::Text("camera state :");
      //todo:
      //ImGui::Text("current cam: %s :", controler.camera.pos_);
      //write current state setting value
      // for (int i = 0; i < temp.size i++
      // )
      // {
      //   ImGui::Text(" [%d] camera name : %s", i, temp[i].cfg.name.c_str());
      // }
      ImGui::EndChild();
    }
    ImGui::End();
  }
}

//void UIRenderer::drawShaderState(ImVec2 size)
//{
//  {
//    ImGui::SetNextWindowPos(ImVec2(20, 200), ImGuiCond_Always);
//    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
//    //ImGui::SetNextWindowSize(size
//    ImGui::Begin("Shader state:", nullptr, ImGuiWindowFlags_NoMove);
//    {
//      ImGui::Text("shader setup:");
//      for (const auto& entry : std::filesystem::directory_iterator(shaderPath_))
//      {
//        if (entry.is_regular_file())
//        {
//          ImGui::Text("%s", entry.path().string().c_str());
//        }
//      }
//      ImGui::End();
//    }
//  }
//}

void UIRenderer::drawMouseState(ImVec2 size)
{
  ImDrawList *draw_list = ImGui::GetForegroundDrawList();
  ImVec2 mouse_pos      = ImGui::GetIO().MousePos;
  float radius          = 20.0f;
  ImU32 mouseColor      = IM_COL32(255, 0, 0, 255);
  float thickness       = 2.0f;
  draw_list->AddCircle(mouse_pos, radius, mouseColor, 32, thickness);
  {
    ImGui::SetNextWindowSize(size, ImGuiCond_Once);
    if (ImGui::Begin("Object"))
    {
      ImGui::TextUnformatted("Compose Mesh + Material");
      ImGui::Separator();
      if (ImGui::Button("Set Object")) {}
      ImGui::SameLine();
      ImGui::TextDisabled("calls setObj()");
    }
    ImGui::End();
  }
}

void UIRenderer::drawFramebufferState()
{
  {
    const ImGuiViewport *vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(vp->Size.x - 200, vp->Pos.y + 10));
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Overlay",
                     nullptr,
                     ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoNav))
    {
      ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
      ImGui::Text("Frame Time: %.2f ms", 1000.0f / ImGui::GetIO().Framerate);
      ImGui::Separator();
    }
    ImGui::End();
  }
}