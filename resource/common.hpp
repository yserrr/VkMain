#ifndef COMMON_HPP
#define COMMON_HPP
//Header
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_XCB_KHR
//std 
#include<stdexcept>
#include<algorithm>
#include<iostream>
#include<cstdlib> 
#include<optional>
#include<cstring>
#include<utility>
#include<vector> 
#include<memory>
#include<set>
#include<map>
//extern
#include<assimp/Importer.hpp>
#include<GLFW/glfw3.h>
#include<vulkan/vulkan.h> 
#include<spdlog/spdlog.h>
#include<glm/glm.hpp>
//commmon
#include<memoryAllocator.hpp>
#endif //COMMON_HPP