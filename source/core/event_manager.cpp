#include<event_manager.hpp>
#include <imgui.h>
//#include<swapchain_manager.hpp>
// #include<imgui.h>
EventManager::EventManager(GLFWwindow *window) :
window_(window)
{
  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, keyCallbackWrapper);
  glfwSetCursorPosCallback(window, cursorPosCallbackWrapper); // 반드시 필요 // 어딘가에서 반드시 등록되어야 함
  glfwSetMouseButtonCallback(window, mouseButtonCallbackWrapper);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetScrollCallback(window, scrollCallback);
  lastActionTime = glfwGetTime(); // 200ms
  spdlog::info("interaction set up");
}

void EventManager::onKeyEvent(int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    if (key == GLFW_KEY_W)
    {
      mainCam->moveForward();
    }
    if (key == GLFW_KEY_S)
    {
      mainCam->moveBackward(); // 예시
    }
    //todo: setting speed
    if (key == GLFW_KEY_P)
    {
//      mainCam->addMoveSpeed(1.0f);
    }
    if (key == GLFW_KEY_O)
    {
//      mainCam->addMoveSpeed(-1.0f);
    }
    if (key == GLFW_KEY_D)
    {
//       mainCam->moveRight();
    }
    if (key == GLFW_KEY_A)
    {
 //      mainCam->moveLeft();
    }
    if (key == GLFW_KEY_Q)
    {
      //glfw should close setting ->end the program
      glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
    if (key == GLFW_KEY_SPACE)
    {
      muliiViews = !muliiViews;
      if (!muliiViews)
      {
        double mouseX, mouseY;
        glfwGetCursorPos(window_, &mouseX, &mouseY);
        getViewIndex(mouseX, mouseY);
      }
    }


    if (key == GLFW_KEY_T)
    {
      mouseMoveState = !mouseMoveState;
    }
  }
}

void EventManager::setCamera(Camera *cam)
{
  mainCam = cam;
}

void EventManager::setSwapchain(SwapchainManager *swapchainP)
{
  ///currentExtent.height = swapchainP->getExtent().height;
  ///currentExtent.width  = swapchainP->getExtent().width;
  ///swapchain_           = swapchainP;
}

void EventManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
  ImGuiIO &io = ImGui::GetIO();
  if (button >= 0 && button < 3)
  {
    io.MouseDown[button] = (action == GLFW_PRESS);
  }

  if (io.WantCaptureMouse)
  {
    return;
  }

  if (button == GLFW_MOUSE_BUTTON_LEFT)
  {
    if (action == GLFW_PRESS)
    {
      leftMousePressed = true;
      glfwGetCursorPos(window, &lastX, &lastY);
      //////todo:
      /////  sculptor mmode setting and rendering method sort
      //switch (curentState)
      //{
      //  case (CurrentManager::Sculptor):
      //  {
      //    Ray ray = mainCam->generateRay(lastX, lastY);
      //    //sculptor-> stroke(lastX, last Y);
      //    break;
      //  }
      //  default:
      //  {
      //    break;
      //  }
      //}
    } else if (action == GLFW_RELEASE)
    {
      leftMousePressed = false;
    }
  }
}

void EventManager::cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
  if (!mouseMoveState) return;
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  float deltaX       = static_cast<float>(xpos - lastX);
  float deltaY       = static_cast<float>(ypos - lastY);
  lastX              = xpos;
  lastY              = ypos;
  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  if (self && self->mainCam)
  {
    //self->mainCam->rotate(-deltaX * sensitivity, -deltaY * sensitivity);
  }
}

void EventManager::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  self->wheelDelta_ += yoffset;
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
  if (!right && !top) index = 0;     // bottom-left
  else if (right && !top) index = 1; // bottom-right
  else if (!right && top) index = 2; // top-left
  else if (right && top) index = 3;  // top-right
  //mainCam->setMainCam(index);
}

void EventManager::wheelUpdate()
{
  //mainCam->fovUpdate(wheelDelta_);
  //wheelDelta_ = 0.0f;
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



//callback
void EventManager::keyCallbackWrapper(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  if (self)
  {
    self->onKeyEvent(key, scancode, action, mods);
  }
}

void EventManager::mouseButtonCallbackWrapper(GLFWwindow *window, int button, int action, int mods)
{
  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  if (self)
  {
    self->mouseButtonCallback(window, button, action, mods);
  }
}

void EventManager::cursorPosCallbackWrapper(GLFWwindow *window, double xpos, double ypos)
{
  ImGuiIO &io = ImGui::GetIO();
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

void EventManager::getKey()
{
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
  {
    mainCam->moveForward(); // 매 프레임 이동
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
  {
    //mainCam->moveLeft(); // 매 프레임 이동
  }
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
  {
    //mainCam->moveRight(); // 매 프레임 이동
  }
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
  {
    mainCam->moveBackward(); // 매 프레임 이동
  }
}