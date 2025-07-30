#include "common.hpp"
#ifndef WINDOW_HPP
#define WINDOW_HPP 
class Window { 
public:  
Window (VkExtent2D& size, const char* title){   
    glfwInit(); 
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); 
    glfwWindowHint(GLFW_RESIZABLE,  GLFW_FALSE); //need to change
    window = glfwCreateWindow(size.width, size.height, title, nullptr, nullptr); 
    spdlog::info("create window");
}
~Window(){ 
    glfwDestroyWindow(window); 
    glfwTerminate();
    spdlog::info("terminate window");
}
GLFWwindow* get() const {return window;}
private: 
GLFWwindow* window; 
}; 
#endif //WINDOW_HPP 