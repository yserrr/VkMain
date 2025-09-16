cmake_minimum_required(VERSION 3.10)
project(MyProject CXX)
project(MyProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)
set(PS_DIR ${SOURCE_DIR}/back/gpu_vk/render_ps)
set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/extern)
set(KTX ${EXTERN_DIR}/ktx/lib)

include_directories(${SOURCE_DIR})
include_directories(${PS_DIR})
include_directories(${SOURCE_DIR}/core)
include_directories(${SOURCE_DIR}/kernel)

include_directories(${SOURCE_DIR}/back/gpu_vk)
include_directories(${SOURCE_DIR}/back/gpu_vk/renderer)
include_directories(${SOURCE_DIR}/back/gpu_vk/batch)
include_directories(${SOURCE_DIR}/back/gpu_vk/device)
include_directories(${SOURCE_DIR}/back/gpu_vk/render_compiler)
include_directories(${SOURCE_DIR}/back/gpu_vk/resources)
include_directories(${SOURCE_DIR}/back/gpu_vk/sync)
include_directories(${SOURCE_DIR}/back/gpu_vk/test)
include_directories(${SOURCE_DIR}/frontend)
include_directories(${SOURCE_DIR}/frontend/lookdev)
include_directories(${SOURCE_DIR}/frontend/scultpor)
include_directories(${SOURCE_DIR}/frontend/geometry)
include_directories(${SOURCE_DIR}/frontend/importer)
include_directories(${SOURCE_DIR}/util)

include_directories(${CMAKE_SOURCE_DIR}/extern)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui/back)
include_directories(${CMAKE_SOURCE_DIR}/extern/assimp)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui)
include_directories(${CMAKE_SOURCE_DIR}/extern/ktx)
include_directories(${CMAKE_SOURCE_DIR}/extern/ktx/include)
include_directories(${CMAKE_SOURCE_DIR}/extern/ktx/lib)
include_directories(${CMAKE_SOURCE_DIR}/extern/ktx/other_include)

include_directories(${CMAKE_SOURCE_DIR}/extern/imgui/back)

find_package(assimp REQUIRED)
find_package(spdlog REQUIRED)
find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SHADERC_PC REQUIRED shaderc)

add_library(common STATIC
        source/back/gpu_vk/resources/memory_allocator.cpp
        source/back/gpu_vk/resources/memory_pool.cpp
        source/back/gpu_vk/renderer/swapchain_manager.cpp

        source/frontend/geometry/mesh.cpp
        source/frontend/geometry/camera.cpp
        source/frontend/importer/importer.cpp
        source/frontend/lookdev/texture.cpp
        source/frontend/lookdev/light.cpp
        source/frontend/geometry/camera_fps.cpp
        source/frontend/geometry/camera_editor.cpp

        source/kernel/event_manager.cpp
        source/kernel/ui_controller.cpp
        source/kernel/ui_renderer_draw.cpp
        source/kernel/ui_renderer_draw_setup.cpp
        source/kernel/ui_renderer_draw_state.cpp

        source/back/gpu_vk/device/context.cpp
        source/back/gpu_vk/device/context_extensions.cpp

        source/back/gpu_vk/render_compiler/command_buffer.cpp
        source/back/gpu_vk/renderer/renderpass.cpp
        source/back/gpu_vk/renderer/pipeline.cpp
        source/back/gpu_vk/renderer/renderer.cpp
        source/back/gpu_vk/renderer/renderer_draw.cpp
        source/back/gpu_vk/renderer/renderer_transfer.cpp
        source/back/gpu_vk/renderer/renderer_init.cpp
        source/back/gpu_vk/renderer/frame_buffer.cpp

        source/back/gpu_vk/resources/image_manager.cpp
        source/back/gpu_vk/resources/descriptor_set.cpp
        source/back/gpu_vk/resources/descriptor_pool.cpp
        source/back/gpu_vk/resources/descriptor_layout_pool.cpp
        source/back/gpu_vk/resources/buffer.cpp
        source/back/gpu_vk/resources/sampler.cpp
        source/back/gpu_vk/resources/streaming_buffer.cpp

        extern/imgui/imgui.cpp
        extern/imgui/imgui_draw.cpp
        extern/imgui/imgui_demo.cpp
        extern/imgui/imgui_tables.cpp
        extern/imgui/imgui_widgets.cpp
        extern/imgui/back/imgui_impl_vulkan.cpp
        extern/imgui/imgui_impl_glfw.cpp
)

target_include_directories(common PUBLIC
        ${SOURCE_DIR}
        ${PS_DIR}
        ${SOURCE_DIR}/core
        ${SOURCE_DIR}/kernel
        ${CMAKE_SOURCE_DIR}/extern/imgui
        ${CMAKE_SOURCE_DIR}/extern/imgui/back
        ${SOURCE_DIR}/back/gpu_vk
        ${SOURCE_DIR}/back/gpu_vk/renderer
        ${SOURCE_DIR}/back/gpu_vk/batch
        ${SOURCE_DIR}/back/gpu_vk/device
        ${SOURCE_DIR}/back/gpu_vk/render_compiler
        ${SOURCE_DIR}/back/gpu_vk/resources
        ${SOURCE_DIR}/back/gpu_vk/sync
        ${SOURCE_DIR}/back/gpu_vk/test
        ${SOURCE_DIR}/frontend
        ${SOURCE_DIR}/frontend/lookdev
        ${SOURCE_DIR}/frontend/scultpor
        ${SOURCE_DIR}/frontend/geometry
        ${SOURCE_DIR}/frontend/importer
        ${SOURCE_DIR}/util
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/extern/assimp
)

target_link_libraries(common PUBLIC
        glfw
        vulkan
        dl
        pthread
        X11
        Xxf86vm
        Xrandr
        Xi
        spdlog::spdlog
        assimp::assimp
        shaderc
)

add_library(ktx STATIC
        ${KTX}/checkheader.c
        ${KTX}/errstr.c
        ${KTX}/etcunpack.cxx
        ${KTX}/filestream.c
        ${KTX}/hashlist.c
        ${KTX}/hashtable.c
        ${KTX}/memstream.c
        ${KTX}/swap.c
        ${KTX}/texture.c
        ${KTX}/vk_funcs.c
        ${KTX}/writer.c
        ${KTX}/writer_v1.c
)
set_target_properties(ktx PROPERTIES LINKER_LANGUAGE C)
add_definitions(-DKTX_USE_KHRONOS_API_VULKAN=1)


add_executable(test ${PS_DIR}/test.cpp)
target_link_libraries(test PRIVATE
        common
        ktx)




