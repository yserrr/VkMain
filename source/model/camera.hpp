// Camera.hpp
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <static_buffer.hpp>
#include <common.hpp>
#include <glm/gtc/quaternion.hpp>
#include <camera_cfg.hpp>

struct CamCI{
  float fov;
  float aspectRatio;
  float nearPlane;
  float farPlane;
};

struct Camera{
  Camera(CamCI info);
  void camUpdate();
  void uploadDescriptor(VkDescriptorSet set);
  void setSpeed(float ds);
  glm::mat4 getViewMatrix() const;
  glm::mat4 getProjectionMatrix() const;
  void setPosition(const glm::vec3 &pos);
  void setDirection(const glm::vec3 &dir);
  void setAspectRatio(float a);
  void moveForward();
  void moveBackward();
  void moveRight();
  void moveLeft();
  void directionReverse();
  void lookAt(glm::vec3 center);
  void addFov(float dt);
  void rotate(float dYawDeg, float dPitDeg);

  Ray generateRay(double posX, double posY);
  VkExtent2D currentExtent;

  alignas (16)struct cameraUBO{
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 camPos;
  } ubo;
  float pitchAccum = 0.0f;
  glm::vec3 pos_;
  glm::vec3 dir_;
  glm::vec3 right_;
  glm::vec3 up_;
  VkDevice device;
  float fov_;
  float aspect;
  float nearPlane;
  float farPlane;
  float delta = 1;
  glm::quat orientation_{};
};

#endif // CAMERA_HPP