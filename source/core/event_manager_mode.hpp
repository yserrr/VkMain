#ifndef MYPROJECT_EVENT_MANAGER_MODE_HPP
#define MYPROJECT_EVENT_MANAGER_MODE_HPP
#include "GLFW/glfw3.h"
#include  <common.hpp>
#include  <camera.hpp>
#include <sculptor.hpp>

enum class ActorMode: uint32_t{
  Sculptor,
  Editor,
  FPS,
  Renderer,
};

struct Actor{
  Actor(GLFWwindow *window) : window_(window) {};
  virtual void act(VkCommandBuffer command) = 0;
  GLFWwindow *window_;
  std::unique_ptr<Sculptor> sculptor;
  float deltaX_     = 0.0;
  float deltaY_     = 0.0;
  float sensitivity = 0.01f;
  bool shoudAct     = false;
  virtual void keyEvent(int key, int scancode, int action, int mods) = 0;
  virtual void cursorPosCallBack(float deltaX, float deltaY) = 0;
  virtual void getKey() =0;
  virtual void getWheelUpdate(float deltaS) = 0;
  virtual void getMouseEvent() =0;
};

struct FPS : Actor{
  FPS(Camera *camera, GLFWwindow *window) : Actor(window) {};
  Camera *mainCam;
  virtual void keyEvent(int key, int scancode, int action, int mods) override {}
  virtual void getMouseEvent() override {}

  virtual void cursorPosCallBack(float deltaX, float deltaY) override
  {
    mainCam->addQuat(-deltaX * sensitivity, -deltaY * sensitivity);
    deltaX = 0;
    deltaY = 0;
  }

  virtual void getKey() override
  {
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
};

struct EditorMode : Actor{};

#endif //MYPROJECT_EVENT_MANAGER_MODE_HPP