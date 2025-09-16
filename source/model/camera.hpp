// Camera.hpp
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <buffer.hpp>
#include <common.hpp>

struct camCreateInfo{ 
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane; 
    MemoryAllocator *allocator;
}; 


class Camera{
public:
Camera(camCreateInfo info);
void camUpdate();
void uploadDescriptor(VkDescriptorSet set);
void setSpeed(float ds){delta += ds;}
glm::mat4 getViewMatrix()       const{return glm::lookAt(position, position + direction, up);}
glm::mat4 getProjectionMatrix() const{return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);}
void setPosition (const glm::vec3 &pos) { position = pos; }
void setDirection(const glm::vec3 &dir) { direction = glm::normalize(dir); }
void setAspectRatio(float a) {aspect = a;}
void moveForward();
void moveBackward(); 
void rotate(float xoffset, float yoffset);
private:
struct cameraUBO{
glm::mat4 view;
glm::mat4 proj;
} ubo;
MemoryAllocator        &allocator;
std::unique_ptr<Buffer> buffer;
glm::vec3 position;
glm::vec3 direction;
glm::vec3 up;
VkDevice device;
float fov;
float aspect;
float nearPlane;
float farPlane;
float delta =4;
float yaw; 
float pitch; 
};

#endif // CAMERA_HPP



