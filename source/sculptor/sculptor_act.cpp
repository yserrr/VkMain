#include "sculptor_act.hpp"
#include "glm/gtx/quaternion.hpp"

constexpr float MODEL_VIEW_TRANSLATE  = 0.01f;
constexpr float MODEL_DEPTH_TRANSLATE = 2.0f;
constexpr float MODEL_SCALING         = 0.01;

/// sculptor don't use camera
/// fixed cam mode

SculptorMode::SculptorMode(GLFWwindow *window, Model *model, VkExtent2D extent) :
  Actor(window),
  extent(extent)
{
  sculptingModel = model;
  sculptor       = std::make_unique<Sculptor>(model);
}

void SculptorMode::keyEvent(int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_D and key == GLFW_KEY_LEFT_CONTROL)
  {
    sculptor->subdivideMesh();
  }
  if (key == GLFW_KEY_F)
  {
    sculptingModel->transform.position   = glm::vec3(0, 0, 0);
    sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();
  }
}

void SculptorMode::getWheelUpdate(float deltaS)
{
  sculptingModel->transform.position -= deltaS * MODEL_DEPTH_TRANSLATE * glm::vec3(0, 0, -1);
  sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();
}

void SculptorMode::act(VkCommandBuffer command)
{
  sculptingModel->mesh->dynMeshUpdate(command);
  shoudAct = false;
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
    if ((glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS))
    {
      glm::quat modelRotate   = sculptingModel->transform.rotation;
      glm::quat pitchRotation = glm::angleAxis(glm::radians(deltaY_), glm::vec3(1.0, 0.0, 0.0));
      glm::quat yawRotation   = glm::angleAxis(glm::radians(deltaX_), glm::vec3(0, 1, 0));
      modelRotate             = yawRotation * pitchRotation * modelRotate;
      modelRotate             = glm::normalize(modelRotate);

      sculptingModel->transform.rotation   = modelRotate;
      sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();
      deltaX_                              = 0;
      deltaY_                              = 0;
      return;
    }
    double x, y;
    glfwGetCursorPos(window_, &x, &y);

    float fragX = (x / extent.width - 0.5) * 2;
    float fragY = (y / extent.height - 0.5) * 2;
    glm::vec3 fragDirection(fragX, fragY, -1);
    fragDirection = glm::normalize(fragDirection);
    glm::mat4 modelMat = sculptingModel->constant.modelMatrix;
    modelMat = glm::inverse(modelMat);

    glm::vec4 direction = glm::vec4(fragDirection, 0.0)* modelMat;
    if (sculptor->stroke( direction))
    {
      shoudAct =true;
    }
    deltaX_ = 0;
    deltaY_ = 0;
    return;
  }
  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
  {
    sculptingModel->transform.position +=
    deltaX_ * MODEL_VIEW_TRANSLATE * glm::vec3(1, 0, 0) -
    deltaY_ * MODEL_VIEW_TRANSLATE * glm::vec3(0, 1, 0);
    sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();

    deltaX_ = 0;
    deltaY_ = 0;
    return;
  }

  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
  {
    sculptingModel->transform.scale += deltaX_ * MODEL_SCALING;
    sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();
    deltaX_                              = 0;
    deltaY_                              = 0;
    return;
  }
}