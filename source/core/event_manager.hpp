#ifndef INTERACTION_HPP
#define INTERACTION_HPP
#include<common.hpp>
#include<camera.hpp>
#include"renderer.hpp"
#include "sculptor.hpp"
#include "event_manager_mode.hpp"

class SwapchainManager;

class EventManager{
  friend class Engine;
public:
  EventManager(GLFWwindow *window);
  void onKeyEvent(int key, int scancode, int action, int mods);
  void setCamera(Camera *cam);
  void setSwapchain(SwapchainManager *swapchainP);
  bool isResized();
  bool isMultiView();
  VkExtent2D getExtent();
  void getKey();
  void getMouseEvent();
  void wheelUpdate();
  void setRenderer(SceneRenderer *renderer);

private:
  static void keyCallbackWrapper(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void mouseButtonCallbackWrapper(GLFWwindow *window, int button, int action, int mods);
  static void cursorPosCallbackWrapper(GLFWwindow *window, double xpos, double ypos);
  static void framebufferSizeCallback(GLFWwindow *window, int w, int h);
  static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
  void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
  void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
  void getViewIndex(double w, double h);

private:
  CurrentActor actor_ = CurrentActor::Sculptor;
  SwapchainManager *swapchain_;
  ResourceManager* resourcesManager_;
  SceneRenderer *renderer_;
  GLFWwindow *window_;
  Camera *mainCam;
  Sculptor sculptor_;
  bool altPressed= false ;
  bool leftButton = false;
  bool middleButton = false;
  bool rightButton = false;


  float yaw   = -90.0f;
  float pitch = 0.0f;
  float distance = 5.0f;


  bool moved = false;
  double wheelDelta_    = 0;
  bool muliiViews       = false;
  bool resized          = false;
  bool leftMousePressed = false;
  float sensitivity     = 0.05f; // 마우스 이동 시 카메라 회전 속도 조절용
  double lastX          = 0.0;
  double lastY          = 0.0;
  double lastActionTime;
  bool mouseMoveState         = true;
  const double actionCooldown = 0.2; // 200ms
  VkExtent2D currentExtent;
};
#endif //INTERACTION_HPP