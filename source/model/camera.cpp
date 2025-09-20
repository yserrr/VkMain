#include<camera.hpp>

static float toRadians(float deg)
{
  return glm::radians(deg);
}

Camera::Camera(CamCI info)
  : pos_(0.0f, 0.0f, 10.0f),
    dir_(0.0f, 0.0f, -1.0f),
    up_(0.0f, 1.0f, 0.0f),
    fov_(info.fov),
    aspect(info.aspectRatio),
    nearPlane(info.nearPlane),
    farPlane(info.farPlane)

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
  ubo.camPos = pos_;
}

void Camera::setSpeed(float ds)
{
  delta += ds;
}

glm::mat4 Camera::getViewMatrix() const
{
  return glm::lookAt(pos_, pos_ + dir_, up_);
}

glm::mat4 Camera::getProjectionMatrix() const
{
  glm::mat4 pers = glm::perspective(glm::radians(fov_), aspect, nearPlane, farPlane);
  pers[1][1] *= -1.0f;
  return pers;
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
  Ray ray{pos_, rayDir};
  return ray;
}

void Camera::moveForward()
{
  pos_ += delta * dir_;
  camUpdate();
}

void Camera::moveBackward()
{
  pos_ -= delta * dir_;
  camUpdate();
}

void Camera::moveRight()
{
  pos_ += delta * right_;
}

void Camera::moveLeft()
{
  pos_ -= delta * right_;
}

void Camera::directionReverse()
{
  dir_ = -dir_;
}

void Camera::lookAt(glm::vec3 center)
{
  dir_ = glm::normalize(center - pos_);
  camUpdate();
}

void Camera::addFov(float dt)
{
  fov_ -= dt;
  camUpdate();
}

void Camera::rotate(float dYawDeg, float dPitDeg)
{
  pitchAccum += dPitDeg;
  pitchAccum = std::clamp(pitchAccum, -89.0f, 89.0f);
  glm::quat pitchRotation = glm::angleAxis(glm::radians(pitchAccum), right_);
  glm::quat yawRotation   = glm::angleAxis(glm::radians(dYawDeg), glm::vec3(0,1,0));
  orientation_ = yawRotation * pitchRotation;
  orientation_ = glm::normalize(orientation_);
  dir_   = glm::normalize(orientation_ * glm::vec3(0,0,-1));
  right_ = glm::normalize(glm::cross(dir_, glm::vec3(0,1,0)));
  if (glm::length(right_) < 1e-6f) right_ = glm::vec3(1,0,0);
  up_    = glm::cross(right_, dir_);
  spdlog::info("direction:: {} {} {}", dir_.x , dir_.y , dir_.z);
  camUpdate();
}