#include "common.hpp"
#ifndef SURFACE_HPP
#define SURFACE_HPP 
class Surface {
public:
Surface(VkInstance instance, GLFWwindow* window) 
:instance(instance), surface(VK_NULL_HANDLE) 
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    spdlog::info("create surface");
}
~Surface() 
{
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }
    spdlog::info("terminate surface");
}
VkSurfaceKHR get() const { return surface; }
private:
VkInstance   instance; 
VkSurfaceKHR surface;
};

#endif //SURFACE_HPP 