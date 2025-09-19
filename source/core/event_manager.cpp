#include<event_manager.hpp>
#include <imgui.h>

EventManager::EventManager(GLFWwindow *window) :
  window_(window)
{
  glfwGetCursorPos(window_, &lastX,&lastY);
  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, keyCallbackWrapper);
  glfwSetCursorPosCallback(window, cursorPosCallbackWrapper);
  glfwSetMouseButtonCallback(window, mouseButtonCallbackWrapper);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetScrollCallback(window, scrollCallback);
  lastActionTime = glfwGetTime();
  spdlog::info("interaction set up");
}

void EventManager::onKeyEvent(int key, int scancode, int action, int mods)
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }

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
    if (key == GLFW_KEY_P)
    {
      //mainCam->addMoveSpeed(1.0f);
    }
    if (key == GLFW_KEY_O)
    {
      //mainCam->(-1.0f);
    }
    if (key == GLFW_KEY_D)
    {
      mainCam->moveRight();
    }
    if (key == GLFW_KEY_D || key == GLFW_KEY_LEFT_CONTROL)
    {
      if (actor_ == CurrentActor::Sculptor)
      {
        sculptor_.subdivideMesh();
      }
    }
    if (key == GLFW_KEY_A)
    {
      mainCam->moveLeft();
    }
    if (key == GLFW_KEY_Q)
    {
      glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }

    if (glfwGetKey(window_, GLFW_KEY_F) == GLFW_PRESS)
    {
      glm::quat quat{};
      mainCam->setPosition(glm::vec3(0.0f, 0.0f, 30.0f));
      mainCam->setDirection(glm::vec3(0.0f, 0.0f, -1.0f));
      mainCam->orientation_ = quat;
      mainCam->camUpdate();
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

void EventManager::setCamera(Camera *cam)
{
  mainCam = cam;
  glfwGetCursorPos(window_, &lastX, &lastY);

  mainCam->addQuaterian(lastX, lastY);
  mainCam->currentExtent = currentExtent;
}

void EventManager::setSwapchain(SwapchainManager *swapchainP)
{
  ///currentExtent.height = swapchainP->getExtent().height;
  ///currentExtent.width  = swapchainP->getExtent().width;
  ///swapchain_= swapchainP;
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
      Ray ray = mainCam->generateRay(lastX, lastY);

      switch (actor_)
      {
        case (CurrentActor::Sculptor):
        {
          //sculptor_.stroke(ray);
          break;
        }
        case (CurrentActor::Editor):
        {
          break ;
        }
        default:
        {
          break;
        }
      }
    } else if (action == GLFW_RELEASE)
    {
      leftMousePressed = false;
    }
  }
}

void EventManager::cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
  float deltaX = static_cast<float>(xpos - lastX);
  float deltaY = static_cast<float>(ypos - lastY);
  lastX        = xpos;
  lastY        = ypos;
  if (!mouseMoveState) return;
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse | moved)
  {
    return;
  }
  EventManager *self = static_cast<EventManager *>(glfwGetWindowUserPointer(window));
  if (self && self->mainCam)
  {
    self->mainCam->addQuaterian(-deltaX * sensitivity, -deltaY * sensitivity);
    moved = true;
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

void EventManager::wheelUpdate()
{
  mainCam->addFov(wheelDelta_);
  wheelDelta_ = 0.0f;
}

void EventManager::setRenderer(SceneRenderer *renderer)
{
  renderer_ = renderer;
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

void EventManager::getKey()
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
  {
    mainCam->moveForward();
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
  {
    mainCam->moveLeft();
  }
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
  {
    mainCam->moveRight();
  }
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
  {
    mainCam->moveBackward();
  }
}

void EventManager::getMouseEvent()
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
  {
    glfwGetCursorPos(window_, &lastX, &lastY);
    Ray ray = mainCam->generateRay(lastX, lastY);
    sculptor_.stroke(ray);
  }
}