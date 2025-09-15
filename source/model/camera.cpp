#include<camera.hpp>

Camera::Camera(camCreateInfo info)
  : position(0.0f, 0.0f, 100.0f),
    direction(0.0f, 0.0f, -1.0f),
    up(0.0f, 1.0f, 0.0f),
    fov(info.fov),
    aspect(info.aspectRatio),
    nearPlane(info.nearPlane),
    farPlane(info.farPlane),
    allocator(*info.allocator),
    yaw(-90.0f),
    pitch(0.0)
{
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

void Camera::moveForward()
{
  position += delta * direction;
  //spdlog::info("move forward");
  camUpdate();
}

void Camera::moveBackward()
{
  position -= delta * direction;
  // spdlog::info("move backward");
  camUpdate();
}

//yaw  = y rotation;
//pitch = x rotation
void Camera::rotate(float xoffset, float yoffset)
{
  float sensitivity = 1.0f;
  yaw += xoffset * sensitivity;
  pitch += yoffset * sensitivity;
  // pitch 제한 (예: -89도 ~ 89도)
  pitch = glm::clamp(pitch, -89.0f, 89.0f);
  glm::vec3 dir;
  dir.x     = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  dir.y     = sin(glm::radians(pitch));
  dir.z     = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction = glm::normalize(dir);
  camUpdate(); // UBO 업데이트
}