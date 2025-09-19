#ifndef MYPROJECT_EVENT_MANAGER_MODE_HPP
#define MYPROJECT_EVENT_MANAGER_MODE_HPP

enum class CurrentActor: uint32_t{
  Sculptor,
  Editor,
  Renderer,
};

struct Ctrl{
  GLFWwindow *window_;
  virtual void keyEvent(int key, int scancode, int action, int mods) = 0;
  virtual void getKey() =0;;
  virtual void getMouseEvent() =0;
};

struct FPSMode : Ctrl{
  Camera *fpsCam;
  virtual void keyEvent(int key, int scancode, int action, int mods) override {}
  virtual void getMouseEvent() override {}

  virtual void getKey() override
  {
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
    {
      fpsCam->moveForward();
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
    {
      fpsCam->moveLeft();
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
    {
      fpsCam->moveRight();
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
    {
      fpsCam->moveBackward();
    }
  }
};

struct SculpingMode : Ctrl{
  Camera *mainCam;
  Sculptor *sculptor_;
  double lastX = 0.0;
  double lastY = 0.0;
  virtual void keyEvent(int key, int scancode, int action, int mods) override;

  virtual void getKey() override
  {
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
      glfwGetCursorPos(window_, &lastX, &lastY);
      Ray ray = mainCam->generateRay(lastX, lastY);
      sculptor_->stroke(ray);
    }
  }
};

#endif //MYPROJECT_EVENT_MANAGER_MODE_HPP