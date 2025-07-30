#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>
#include <stdexcept>
#ifndef SHADERMOUDULE_HPP
#define SHADERMOUDULE_HPP 

class ShaderModule {
public:
ShaderModule(VkDevice device, const char* filepath) : device(device), shaderModule(VK_NULL_HANDLE) {
// 파일 읽기
std::ifstream file(filepath, std::ios::ate | std::ios::binary);
spdlog::info("shader path: {}",filepath);
if (!file.is_open()) {
    throw std::runtime_error("failed to open shader file!");
}
size_t fileSize = (size_t)file.tellg();
std::vector<char> buffer(fileSize);
file.seekg(0);
file.read(buffer.data(), fileSize);
file.close();
// ShaderModule 생성 정보 설정
VkShaderModuleCreateInfo createInfo{};
createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
createInfo.codeSize = buffer.size();
createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
}
}
~ShaderModule() {
if (shaderModule != VK_NULL_HANDLE) {
    vkDestroyShaderModule(device, shaderModule, nullptr);
}
}

VkShaderModule get() const { return shaderModule; }
private:
VkDevice       device;
VkShaderModule shaderModule;
};

#endif 