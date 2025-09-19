#include<camera.hpp>

static float toRadians(float deg)
{
  return glm::radians(deg);
}

Camera::Camera(CamCI info)
  : position_(0.0f, 0.0f, 100.0f),
    dir_(0.0f, 0.0f, -1.0f),
    up_(0.0f, 1.0f, 0.0f),
    fov_(info.fov),
    aspect(info.aspectRatio),
    nearPlane(info.nearPlane),
    farPlane(info.farPlane),
    yaw(-90.0f),
    pitch(0.0)
{
  right_ = glm::normalize(glm::cross(dir_, up_));
  right_ = glm::normalize(right_);
  camUpdate();
  spdlog::info("set camera");
}

void Camera::camUpdate()
{
  ubo.view = getViewMatrix();
  ubo.proj = getProjectionMatrix();
}

void Camera::setSpeed(float ds)
{
  delta += ds;
}

glm::mat4 Camera::getViewMatrix() const
{
  return glm::lookAt(position_, position_ + dir_, up_);
}

glm::mat4 Camera::getProjectionMatrix() const
{
  glm::mat4 pers = glm::perspective(glm::radians(fov_), aspect, nearPlane, farPlane);
  pers[1][1] *= -1.0f;
  return pers;
}

void Camera::setPosition(const glm::vec3 &pos)
{
  position_ = pos;
}

void Camera::setDirection(const glm::vec3 &dir)
{
  dir_ = glm::normalize(dir);
}

void Camera::setAspectRatio(float a)
{
  aspect = a;
}

Ray Camera::generateRay(double posX, double posY)
{
  double xNdc = (2 * posX / currentExtent.width) - 1;
  double yNdc = 1 - (2 * posY / currentExtent.height);

  float tanFov     = tan(fov_ * 0.5f);
  float xView      = xNdc * aspect* tanFov;
  float yView      = yNdc * tanFov;
  glm::vec3 rayDir = glm::vec3(-xView, yView, -1.0f);
  rayDir = glm::normalize(rayDir);
  std::cout << rayDir.x << " " << rayDir.y << " " << rayDir.z << std::endl;
  Ray ray{position_, rayDir};
  return ray;
}

void Camera::moveForward()
{
  position_ += delta * dir_;
  camUpdate();
}

void Camera::moveBackward()
{
  position_ -= delta * dir_;
  camUpdate();
}

void Camera::moveRight()
{
  position_ += delta * right_;
}

void Camera::moveLeft()
{
  position_ -= delta * right_;
}

void Camera::directionReverse()
{
  dir_ = -dir_;
}

void Camera::addFov(float dt)
{
  fov_ -= dt;
  camUpdate();
}

void Camera::addQuaterian(float dYawDeg, float dPitDeg)
{
  glm::quat pitchRotation = glm::angleAxis(toRadians(dPitDeg), right_);             // 현재 right_ 축 기준
  glm::quat yawRotation   = glm::angleAxis(toRadians(dYawDeg), glm::vec3(0, 1, 0)); // 월드 Y축 기준
  orientation_            = yawRotation * pitchRotation * orientation_;             // 새로운 회전을 기존 회전에 곱함 (순서 중요)
  orientation_            = glm::normalize(orientation_);

  dir_   = glm::normalize(orientation_ * glm::vec3(0, 0, -1));
  up_    = glm::normalize(orientation_ * glm::vec3(0, 1, 0));
  right_ = glm::normalize(glm::cross(dir_, up_));
  camUpdate();
}