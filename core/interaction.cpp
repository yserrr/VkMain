#include<interaction.hpp>


Interaction::Interaction (GLFWwindow* window)
: 
window(window)
{
glfwSetWindowUserPointer(window, this);
glfwSetKeyCallback(window, keyCallbackWrapper);
glfwSetCursorPosCallback(window, cursorPosCallbackWrapper); // 반드시 필요 // 어딘가에서 반드시 등록되어야 함
glfwSetMouseButtonCallback(window, mouseButtonCallbackWrapper);
lastActionTime = glfwGetTime();  // 200ms
spdlog::info("interaction set up");
}
void Interaction::onKeyEvent(int key, int scancode, int action, int mods){ 
 if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            mainCam->moveForward();
        }
        if (key == GLFW_KEY_S) {
            mainCam->moveBackward();  // 예시
        }
        if (key == GLFW_KEY_O){
            mainCam->setSpeed(1.0f);
        }
        if (key == GLFW_KEY_I){
            mainCam->setSpeed(-1.0f);
        }
    }
}
void Interaction::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftMousePressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else if (action == GLFW_RELEASE) {
            leftMousePressed = false;
        }
    }
}

void Interaction::cursorPosCallback(GLFWwindow* window, double xpos, double ypos){
if (leftMousePressed) {
        float deltaX = static_cast<float>(xpos - lastX);
        float deltaY = static_cast<float>(ypos - lastY);

        lastX = xpos;
        lastY = ypos;
        // Camera 클래스에 회전 함수가 있다고 가정
        Interaction* self = static_cast<Interaction*>(glfwGetWindowUserPointer(window));
        if (self && self->mainCam) {
            self->mainCam->rotate(deltaX * sensitivity, deltaY * sensitivity);
        }
    }
}