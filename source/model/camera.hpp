// Camera.hpp
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <static_buffer.hpp>
#include <common.hpp>
#include <glm/gtc/quaternion.hpp>

struct camCreateInfo{
  float fov;
  float aspectRatio;
  float nearPlane;
  float farPlane;
  MemoryAllocator *allocator;
};

class Camera{
  friend class ResourceManager;

public:
  Camera(camCreateInfo info);
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
  void addFov(float dt);
  void addQuaterian(float dYawDeg, float dPitDeg);

private:
  struct cameraUBO{
    glm::mat4 view;
    glm::mat4 proj;
  } ubo;

  MemoryAllocator &allocator;
  std::unique_ptr<StaticBuffer> buffer;
  glm::vec3 position;
  glm::vec3 direction_;
  glm::vec3 right_;
  glm::vec3 up;
  VkDevice device;
  float fov;
  float aspect;
  float nearPlane;
  float farPlane;
  float delta = 1;
  float yaw;
  float pitch;
  glm::quat orientation_{};
};

#endif // CAMERA_HPP