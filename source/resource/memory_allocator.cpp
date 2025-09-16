//
// Created by ljh on 25. 9. 16..
//
#include "memory_allocator.hpp"
#include "spdlog/spdlog.h"

extern PFN_vkSetDebugUtilsObjectNameEXT g_pfnSetDebugUtilsObjectNameEXT; // for VkDevice objects
extern PFN_vkSetDebugUtilsObjectTagEXT  g_pfnSetDebugUtilsObjectTagEXT;  // if using tags
// 필요한 다른 디버그 유틸리티 함수 포인터들도 extern으로 선언
// 예: PFN_vkCmdBeginDebugUtilsLabelEXT, PFN_vkCmdEndDebugUtilsLabelEXT 등
namespace VulkanObjectHelpers{
  template<typename HandleType>
  VkObjectType GetVulkanObjectType(HandleType handle); // 포워드 선언
  // 특수화 (템플릿 오버로딩 또는 직접 정의)
  template<> VkObjectType GetVulkanObjectType(VkBuffer)
  {
    return VK_OBJECT_TYPE_BUFFER;
  }

  template<> VkObjectType GetVulkanObjectType(VkImage)
  {
    return VK_OBJECT_TYPE_IMAGE;
  }

  template<> VkObjectType GetVulkanObjectType(VkDeviceMemory)
  {
    return VK_OBJECT_TYPE_DEVICE_MEMORY;
  }

  template<> VkObjectType GetVulkanObjectType(VkSampler)
  {
    return VK_OBJECT_TYPE_SAMPLER;
  }

  template<> VkObjectType GetVulkanObjectType(VkImageView)
  {
    return VK_OBJECT_TYPE_IMAGE_VIEW;
  }

  template<> VkObjectType GetVulkanObjectType(VkBufferView)
  {
    return VK_OBJECT_TYPE_BUFFER_VIEW;
  }

  template<> VkObjectType GetVulkanObjectType(VkCommandPool)
  {
    return VK_OBJECT_TYPE_COMMAND_POOL;
  }

  template<> VkObjectType GetVulkanObjectType(VkCommandBuffer)
  {
    return VK_OBJECT_TYPE_COMMAND_BUFFER;
  }

  template<> VkObjectType GetVulkanObjectType(VkDescriptorPool)
  {
    return VK_OBJECT_TYPE_DESCRIPTOR_POOL;
  }

  template<> VkObjectType GetVulkanObjectType(VkDescriptorSet)
  {
    return VK_OBJECT_TYPE_DESCRIPTOR_SET;
  }

  template<> VkObjectType GetVulkanObjectType(VkDescriptorSetLayout)
  {
    return VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
  }

  template<> VkObjectType GetVulkanObjectType(VkFence)
  {
    return VK_OBJECT_TYPE_FENCE;
  }

  template<> VkObjectType GetVulkanObjectType(VkSemaphore)
  {
    return VK_OBJECT_TYPE_SEMAPHORE;
  }

  template<> VkObjectType GetVulkanObjectType(VkEvent)
  {
    return VK_OBJECT_TYPE_EVENT;
  }

  template<> VkObjectType GetVulkanObjectType(VkQueryPool)
  {
    return VK_OBJECT_TYPE_QUERY_POOL;
  }

  template<> VkObjectType GetVulkanObjectType(VkFramebuffer)
  {
    return VK_OBJECT_TYPE_FRAMEBUFFER;
  }

  template<> VkObjectType GetVulkanObjectType(VkRenderPass)
  {
    return VK_OBJECT_TYPE_RENDER_PASS;
  }

  template<> VkObjectType GetVulkanObjectType(VkPipeline)
  {
    return VK_OBJECT_TYPE_PIPELINE;
  }

  template<> VkObjectType GetVulkanObjectType(VkPipelineLayout)
  {
    return VK_OBJECT_TYPE_PIPELINE_LAYOUT;
  }

  template<> VkObjectType GetVulkanObjectType(VkShaderModule)
  {
    return VK_OBJECT_TYPE_SHADER_MODULE;
  }

  //template <> VkObjectType GetVulkanObjectType(VkImageView)         { return VK_OBJECT_TYPE_IMAGE_VIEW;             }
  template<> VkObjectType GetVulkanObjectType(VkQueue)
  {
    return VK_OBJECT_TYPE_QUEUE;
  }

  // 헬퍼: Vulkan 객체에 디버그 이름 설정
  template<typename HandleType>
  void SetVulkanObjectName(VkDevice device, HandleType handle, const std::string &name)
  {
    if (g_pfnSetDebugUtilsObjectNameEXT != nullptr)
    {
      VkDebugUtilsObjectNameInfoEXT nameInfo{};
      nameInfo.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
      nameInfo.objectType   = GetVulkanObjectType(handle);
      nameInfo.objectHandle = reinterpret_cast<uint64_t>(handle);
      nameInfo.pObjectName  = name.c_str();
      //g_pfnSetDebugUtilsObjectNameEXT(device, &nameInfo);
    }
  }
};

MemoryAllocator::MemoryAllocator(VkPhysicalDevice physicalDevice, VkDevice device) : physicalDevice(physicalDevice),
                                                                                     device(device) {}

MemoryAllocator::~MemoryAllocator()
{
  for (auto *pool: pools)
  {
    if (pool != VK_NULL_HANDLE) delete pool;
  }
}

Allocation MemoryAllocator::allocate(VkMemoryRequirements  requirements,
                                     VkMemoryPropertyFlags desiredFlags,
                                     const std::string &   debugName)
{
  uint32_t memoryType = findMemoryType(requirements.memoryTypeBits, desiredFlags);
  for (auto *pool: pools)
  {
    if (pool->getMemoryTypeIndex() == memoryType)
    {
      Allocation result;
      if (pool->allocate(requirements.size, requirements.alignment, result))
      {
        return result;
      }
    }
  }
  VkDeviceSize poolSize = std::max(requirements.size * 8, (VkDeviceSize) 256 * 1024 * 1024); // 256MB 기본
  MemoryPool * newPool  = new MemoryPool(device, memoryType, poolSize);
  pools.push_back(newPool);
  Allocation result;
  if (!newPool->allocate(requirements.size, requirements.alignment, result))
  {
    throw std::runtime_error("Failed to allocate memory from new pool");
  }
  if (desiredFlags != VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
  {
    result.maped = newPool->map();
  }
  return result;
}

void MemoryAllocator::free(Allocation allocation, VkDeviceSize size)
{
  for (auto *pool: pools)
  {
    if (pool->getMemory() == allocation.memory)
    {
      pool->free(allocation.offset, size);
      return;
    }
  }
  throw std::runtime_error("Tried to free memory from unknown pool");
}

VkDevice MemoryAllocator::getDevice()
{
  return device;
}

VkPhysicalDevice MemoryAllocator::getPhysicalDevice()
{
  return physicalDevice;
}

uint32_t MemoryAllocator::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
  {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
    {
      return i;
    }
  }
  throw std::runtime_error("Failed to find suitable memory type");
}

// 버퍼 생성 및 메모리 할당/바인딩
VkResult MemoryAllocator::createBuffer(const VkBufferCreateInfo *pCreateInfo,
                                       VkMemoryPropertyFlags     desiredFlags,
                                       VkBuffer *                pBuffer,
                                       Allocation *              pAllocation,
                                       const std::string &       debugName)
{
  //VkBuffer 핸들 생성
  VkResult result = vkCreateBuffer(device, pCreateInfo, nullptr, pBuffer);
  if (result != VK_SUCCESS)
  {
    spdlog::error("Failed to create VkBuffer '{}'! Result: {}", debugName, static_cast<int>(result));
    return result;
  }
  //해당 VkBuffer에 필요한 메모리 요구사항 조회
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, *pBuffer, &memRequirements);
  // MemoryAllocator를 통해 실제 GPU 메모리 할당
  try
  {
    *pAllocation = allocate(memRequirements, desiredFlags, debugName); // 할당 시점에 디버그 이름 전달
  } catch (const std::runtime_error &e)
  {
    spdlog::error("Failed to allocate memory for VkBuffer '{}'! Error: {}", debugName, e.what());
    vkDestroyBuffer(device, *pBuffer, nullptr); // 버퍼 생성 취소
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;       // 또는 다른 적절한 에러 코드
  }
  //  VkBuffer 객체에 할당된 메모리 바인딩
  result = vkBindBufferMemory(device, *pBuffer, pAllocation->memory, pAllocation->offset);
  if (result != VK_SUCCESS)
  {
    spdlog::error("Failed to bind memory to VkBuffer '{}'! Result: {}", debugName, static_cast<int>(result));
    free(*pAllocation, pAllocation->size);      // 할당된 메모리 해제
    vkDestroyBuffer(device, *pBuffer, nullptr); // 버퍼 객체 파괴
    return result;
  }
  // 디버그 이름 설정 (확장이 활성화되어 있다면)
  VulkanObjectHelpers::SetVulkanObjectName(device, *pBuffer, debugName);
  VulkanObjectHelpers::SetVulkanObjectName(device, pAllocation->memory, debugName + "_Mem"); // 할당된 메모리에도 이름 붙이기
  pAllocation->type = AllocationType::BUFFER;                                                // Allocation 타입 설정
  spdlog::info("VkBuffer '{}' created and memory allocated/bound successfully. Size: {}, Offset: {}, MemType: {}",
               debugName,
               pCreateInfo->size,
               pAllocation->offset,
               pAllocation->memoryTypeIndex);

  return VK_SUCCESS;
}

VkResult MemoryAllocator::createImage(const VkImageCreateInfo *pCreateInfo,
                                      VkMemoryPropertyFlags    desiredFlags,
                                      VkImage *                pImage,
                                      Allocation *             pAllocation,
                                      const std::string &      debugName)
{
  VkResult result = vkCreateImage(device, pCreateInfo, nullptr, pImage);
  if (result != VK_SUCCESS)
  {
    spdlog::error("Failed to create VkImage '{}'! Result: {}", debugName, static_cast<int>(result));
    return result;
  }
  //  해당 VkImage에 필요한 메모리 요구사항 조회
  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, *pImage, &memRequirements);
  //  MemoryAllocator를 통해 실제 GPU 메모리 할당
  try
  {
    *pAllocation = allocate(memRequirements, desiredFlags, debugName);
  } catch (const std::runtime_error &e)
  {
    spdlog::error("Failed to allocate memory for VkImage '{}'! Error: {}", debugName, e.what());
    vkDestroyImage(device, *pImage, nullptr);
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
  }
  result = vkBindImageMemory(device, *pImage, pAllocation->memory, pAllocation->offset);
  if (result != VK_SUCCESS)
  {
    free(*pAllocation, pAllocation->size);
    throw std::runtime_error("Failed to create VkImage '{}'! Result: {}");
    spdlog::error("Failed to bind memory to VkImage '{}'! Result: {}", debugName, static_cast<int>(result));
    vkDestroyImage(device, *pImage, nullptr);
    return result;
  }

  // 5. 디버그 이름 설정
  VulkanObjectHelpers::SetVulkanObjectName(device, *pImage, debugName);
  VulkanObjectHelpers::SetVulkanObjectName(device, pAllocation->memory, debugName + "_Mem");
  pAllocation->type = AllocationType::IMAGE; // Allocation 타입 설정
  spdlog::info(
               "VkImage '{}' created and memory allocated/bound successfully. Extent: {}x{}x{}, Offset: {}, MemType: {}",
               debugName,
               pCreateInfo->extent.width,
               pCreateInfo->extent.height,
               pCreateInfo->extent.depth,
               pAllocation->offset,
               pAllocation->memoryTypeIndex);
  return VK_SUCCESS;
}

void MemoryAllocator::destroyBuffer(VkBuffer buffer, const Allocation &allocation)
{
  if (VK_VALID(buffer))
  {
    vkDestroyBuffer(device, buffer, nullptr);
    VulkanObjectHelpers::SetVulkanObjectName(device, buffer, ""); // 디버그 이름 초기화
  }
  //if allocation mapped -> free
  if (VK_VALID(allocation.memory))
  {
    free(allocation,allocation.size);
  }
  spdlog::info("VkBuffer '{}' and its memory freed.", allocation.debugName);
}

void MemoryAllocator::destroyImage(VkImage image, const Allocation &allocation)
{
  if (VK_VALID(image))
  {
    vkDestroyImage(device, image, nullptr);
    VulkanObjectHelpers::SetVulkanObjectName(device, image, "");
  }
  if (VK_VALID(allocation.memory))
  {
    free(allocation, allocation.size);
  }
  spdlog::info("VkImage '{}' and its memory freed.", allocation.debugName);
}