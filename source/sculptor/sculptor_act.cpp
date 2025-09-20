#include "sculptor_act.hpp"

SculptorMode::SculptorMode(Camera *camera, GLFWwindow *window) : Actor(camera, window)
{
  sculptor = std::make_unique<Sculptor>();
}

void SculptorMode::keyEvent(int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_D || key == GLFW_KEY_LEFT_CONTROL)
  {
    sculptor->subdivideMesh();
  }
  if (key == GLFW_KEY_F)
  {
    mainCam->lookAt(glm::vec3(0));
    spdlog::info ("camera direciton :: {} {} {}", mainCam->dir_.x , mainCam->dir_.y, mainCam->dir_.z);
  }
}

void SculptorMode::cursorPosCallBack(float deltaX, float deltaY)
{
  deltaX_ = deltaX;
  deltaY_ = deltaY;
}

void SculptorMode::getKey() {}

void SculptorMode::getMouseEvent()
{
  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
  {
    if (glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
      mainCam ->rotate(deltaX_ *0.01, deltaY_ *0.01);
      deltaX_ = 0;
      deltaY_ = 0;
    } else
    {
      double x, y;
      glfwGetCursorPos(window_, &x, &y);
      Ray ray = mainCam->generateRay(x, y);
      sculptor->stroke(ray);
      deltaX_ = 0;
      deltaY_ = 0;
      return;
    }
  }
   if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
   {
     mainCam->pos_ -= deltaX_ * sensitivity * mainCam->right_
                    - deltaY_ * sensitivity * mainCam->up_;
     deltaX_ = 0;
     deltaY_ = 0;
     return;
   }
  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
  {
    mainCam->pos_.z += 0.05f* deltaY_;
    deltaX_ = 0;
    deltaY_ = 0;
    return;
  }
  deltaX_ = 0;
  deltaY_ = 0;
}