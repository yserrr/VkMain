#include<common.hpp> 
#include<camera.hpp>
#ifndef INTERACTION_HPP
#define INTERACTION_HPP 
class Interaction {
public:
Interaction(GLFWwindow* window);
void onKeyEvent(int key, int scancode, int action, int mods);
void setCamera (Camera* cam){ mainCam = cam;}
private:
GLFWwindow* window;
Camera*     mainCam;
// 전역 또는 App class 내
bool  leftMousePressed = false;
double lastX = 0.0;
double lastY = 0.0;

// 마우스 이동 시 카메라 회전 속도 조절용
float sensitivity = 0.001f;
static void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Interaction* self = static_cast<Interaction*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->onKeyEvent(key, scancode, action, mods);
    }
}

static void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods) {
    Interaction* self = static_cast<Interaction*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->mouseButtonCallback(window, button, action, mods);
    }
}

static void cursorPosCallbackWrapper(GLFWwindow* window, double xpos, double ypos) {
    Interaction* self = static_cast<Interaction*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->cursorPosCallback(window, xpos, ypos);
    }
}

// GLFW 마우스 버튼 콜백
// GLFW 커서 이동 콜백
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback  (GLFWwindow* window, double xpos, double ypos);
double lastActionTime ;
const double actionCooldown = 0.2;  // 200ms
};

#endif //INTERACTION_HPP