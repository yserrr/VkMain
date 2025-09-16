//
// Created by ljh on 25. 9. 1..
//

#ifndef MYPROJECT_TEST_MODULE_HPP
#define MYPROJECT_TEST_MODULE_HPP
#include <device.hpp>
#include <memory_allocator.hpp>
#include <swapchain_manager.hpp>
#include <import_desc.hpp>
#include <importer.hpp>
#include "../../util/common.hpp"
#include <mesh.hpp>
#include <shader_module.hpp>
#include <pipeline.hpp>
#include <command_buffer.hpp>
#include <command_pool.hpp>
#include <renderpass.hpp>
#include <../resources/descriptor_layout_pool.hpp>
#include <../resources/descriptor_set.hpp>
#include <semaphore_pool.hpp>
#include <fence.hpp>
#include <buffer.hpp>
#include <streaming_buffer.hpp>
#include <camera.hpp>
#include <event_manager.hpp>
#include <texture.hpp>
#include <image_manager.hpp>
#include <sampler.hpp>
#include <ui_renderer.hpp>
#include <light.hpp>


//void draw(){
//  for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
//  {
//    const VkMemoryType& type = memProps.memoryTypes[i];
//    spdlog::info("MemoryType {}: propertyFlags = {}", i, type.propertyFlags);
//  }
//
//  for (uint32_t i = 0; i < memProps.memoryHeapCount; ++i)
//  {
//    const VkMemoryHeap& heap = memProps.memoryHeaps[i];
//    spdlog::info("MemoryHeap {}: size = {} MB, flags = {}", i, heap.size / (1024 * 1024), heap.flags);
//  }
//
//}



#endif //MYPROJECT_TEST_MODULE_HPP


