#include<event_manager.hpp>
#include "sculptor_act.hpp"
#include "imgui_impl_glfw.h"
#include <imgui.h>

EventManager::EventManager(GLFWwindow *window, Camera *mainCam, ResourceManager *resourceManager) :
  window_(window),
  mainCam(mainCam),
  resourcesManager_(resourceManager)
{
  syncWithCam(mainCam);
  glfwGetCursorPos(window_, &lastX, &lastY);
  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, keyCallbackWrapper);
  glfwSetCursorPosCallback(window, cursorPosCallbackWrapper);
  glfwSetMouseButtonCallback(window, mouseButtonCallbackWrapper);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetScrollCallback(window, scrollCallback);

//  glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
//  glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
//  glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
//  glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
//
  lastActionTime = glfwGetTime();
  spdlog::info("interaction set up");
  createActor();
}

void EventManager::onKeyEvent(int key, int scancode, int action, int mods)
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  actor_->keyEvent(key, scancode, action, mods);
  if (action == GLFW_PRESS)
  {
    if (key == GLFW_KEY_1)
    {
      renderer_->polygonMode = VK_POLYGON_MODE_FILL;
    }
    if (key == GLFW_KEY_2)
    {
      renderer_->polygonMode = VK_POLYGON_MODE_LINE;
    }
    if (key == GLFW_KEY_3)
    {
      renderer_->depthTest = !renderer_->depthTest;
    }
    if (key == GLFW_KEY_Q)
    {
      glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
    if (key == GLFW_KEY_SPACE)
    {
      muliiViews = !muliiViews;
      if (!muliiViews)
      {
        renderer_->viewMode = ViewMode::SingleView;
        double mouseX, mouseY;
        glfwGetCursorPos(window_, &mouseX, &mouseY);
        getViewIndex(mouseX, mouseY);
      } else
      {
        renderer_->viewMode = ViewMode::MultiView;
      }
    }

    if (key == GLFW_KEY_T)
    {
      mouseMoveState = !mouseMoveState;
      if (mouseMoveState)
      {
        io.MousePos.x = lastX;
        io.MousePos.y = lastY;
      }
    }
  }
}

void EventManager::syncWithCam(Camera *cam)
{
  glm::quat quat{};
  mainCam->pos_         = glm::vec3(0.0f, 0.0f, 30.0f);
  mainCam->dir_         = glm::vec3(0.0f, 0.0f, -1.0f);
  mainCam->orientation_ = quat;
  mainCam->camUpdate();
  lastX = 0;
  lastY = 0;

  glfwGetCursorPos(window_, &lastX, &lastY);
  mainCam->rotate(lastX, lastY);
  mainCam->currentExtent = currentExtent;
}

void EventManager::setSwapchain(SwapchainManager *swapchainP)
{
  ///currentExtent.height = swapchainP->getExtent().height;
  ///currentExtent.width  = swapchainP->getExtent().width;
  ///swapchain_= swapchainP;
}

bool EventManager::isResized()
{
  if (resized)
  {
    resized = false;
    return true;
  }
  return false;
}

bool EventManager::isMultiView()
{
  return muliiViews;
}

VkExtent2D EventManager::getExtent()
{
  return currentExtent;
}

void EventManager::getKey()
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  actor_->getKey();
}

void EventManager::getMouseEvent()
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  actor_->getMouseEvent();
}

void EventManager::wheelUpdate()
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  mainCam->addFov(wheelDelta_);
  wheelDelta_ = 0.0f;
}

void EventManager::setRenderer(SceneRenderer *renderer)
{
  renderer_ = renderer;
}

void EventManager::createActor()
{
  switch (currentActor_)
  {
    case(ActorMode::Sculptor):
    {
      actor_                  = std::make_unique<SculptorMode>(mainCam, window_);
      actor_->sculptor->model = &resourcesManager_->selectedModel;
      break;
    }
    default:
    {
      break;
    }
  }
}

void EventManager::keyCallbackWrapper(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  if (self)
  {
    self->onKeyEvent(key, scancode, action, mods);
  }
}

void EventManager::mouseButtonCallbackWrapper(GLFWwindow *window, int button, int action, int mods)
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }

  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  if (self)
  {
    self->mouseButtonCallback(window, button, action, mods);
  }
}

void EventManager::cursorPosCallbackWrapper(GLFWwindow *window, double xpos, double ypos)
{
  ImGuiIO &io   = ImGui::GetIO();
  io.MousePos.x = static_cast<float>(xpos);
  io.MousePos.y = static_cast<float>(ypos);
  if (io.WantCaptureMouse)
  {
    return;
  }

  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  if (self)
  {
    self->cursorPosCallback(window, xpos, ypos);
  }
}

void EventManager::framebufferSizeCallback(GLFWwindow *window, int w, int h)
{
  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  if (self)
  {
    self->resized              = true;
    self->currentExtent.height = h;
    self->currentExtent.width  = w;
  }
}

void EventManager::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  self->wheelDelta_ += yoffset;
}

void EventManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
  ImGuiIO &io          = ImGui::GetIO();
  io.MouseDown[button] = true;
  if (io.WantCaptureMouse)
  {
    return;
  }
}

void EventManager::cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
  float deltaX = static_cast<float>(xpos - lastX);
  float deltaY = static_cast<float>(ypos - lastY);
  lastX        = xpos;
  lastY        = ypos;
  if (!mouseMoveState) return;
  ImGuiIO &io   = ImGui::GetIO();
  io.MousePos.x = static_cast<float>(xpos);
  io.MousePos.y = static_cast<float>(ypos);
  if (io.WantCaptureMouse | moved)
  {
    return;
  }
  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  actor_->cursorPosCallBack(deltaX, deltaY);
  moved = true;
}

void EventManager::getViewIndex(double w, double h)
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  bool right = w >= (currentExtent.width / 2.0f);
  bool top   = h >= (currentExtent.height / 2.0f);
  int index  = 0;
  if (!right && !top) index = 0;
  else if (right && !top) index = 1;
  else if (!right && top) index = 2;
  else if (right && top) index = 3;
  //mainCam->setMainCam(index);
}