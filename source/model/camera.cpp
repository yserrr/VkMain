#include<camera.hpp>

static float toRadians(float deg)
{
  return glm::radians(deg);
}

Camera::Camera(camCreateInfo info)
  : position(0.0f, 0.0f, 100.0f),
    direction_(0.0f, 0.0f, -1.0f),
    up(0.0f, 1.0f, 0.0f),
    fov(info.fov),
    aspect(info.aspectRatio),
    nearPlane(info.nearPlane),
    farPlane(info.farPlane),
    allocator(*info.allocator),
    yaw(-90.0f),
    pitch(0.0)
{
  right_ = glm::normalize(glm::cross(direction_, up));
  right_ = glm::normalize(right_);

  device = allocator.getDevice();
  buffer = std::make_unique<Buffer>(allocator, sizeof(cameraUBO), BufferType::Uniform);
  buffer->createUniformBuffer();
  camUpdate();
  spdlog::info("set camera");
}

void Camera::camUpdate()
{
  ubo.view = getViewMatrix();
  ubo.proj = getProjectionMatrix();
  buffer->loadData(&ubo, sizeof(cameraUBO));
}

//upload camera descriptors for camera information
void Camera::uploadDescriptor(VkDescriptorSet set)
{
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = buffer->getBuffer()[0];
  bufferInfo.offset = 0;
  bufferInfo.range  = sizeof(ubo);
  VkWriteDescriptorSet descriptorWrite{};
  descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet          = set;
  descriptorWrite.dstBinding      = 0;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pBufferInfo     = &bufferInfo;
  vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void Camera::setSpeed(float ds)
{
  delta += ds;
}

glm::mat4 Camera::getViewMatrix() const
{
  return glm::lookAt(position, position + direction_, up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
  glm::mat4 pers = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
  pers[1][1] *= -1.0f;
  return pers;
}

void Camera::setPosition(const glm::vec3 &pos)
{
  position = pos;
}

void Camera::setDirection(const glm::vec3 &dir)
{
  direction_ = glm::normalize(dir);
}

void Camera::setAspectRatio(float a)
{
  aspect = a;
}

void Camera::moveForward()
{
  position += delta * direction_;
  camUpdate();
}

void Camera::moveBackward()
{
  position -= delta * direction_;
  camUpdate();
}

void Camera::moveRight()
{
  position += delta * right_;
}

void Camera::moveLeft()
{
  position -= delta * right_;
}

void Camera::directionReverse()
{
  direction_ = -direction_;
}

void Camera::addFov(float dt)
{
  fov -= dt;
  camUpdate();
}

void Camera::addQuaterian(float dYawDeg, float dPitDeg)
{
  glm::quat pitchRotation = glm::angleAxis(toRadians(dPitDeg), right_);             // 현재 right_ 축 기준
  glm::quat yawRotation   = glm::angleAxis(toRadians(dYawDeg), glm::vec3(0, 1, 0)); // 월드 Y축 기준
  orientation_            = yawRotation * pitchRotation * orientation_;             // 새로운 회전을 기존 회전에 곱함 (순서 중요)
  orientation_            = glm::normalize(orientation_);
  direction_              = glm::normalize(orientation_ * glm::vec3(0, 0, -1));
  camUpdate();
}