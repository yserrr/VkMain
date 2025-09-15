
cmake_minimum_required(VERSION 3.10)
project(MyProject CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

include_directories(${CMAKE_SOURCE_DIR}/source/resource)
include_directories(${CMAKE_SOURCE_DIR}/source/renderer)
include_directories(${CMAKE_SOURCE_DIR}/source/model)
include_directories(${CMAKE_SOURCE_DIR}/source/core)

include_directories(${CMAKE_SOURCE_DIR}/extern)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui/back)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui/back)

find_package(assimp REQUIRED)
find_package(spdlog REQUIRED)
find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SHADERC_PC REQUIRED shaderc)

add_executable(exe
        main.cpp

        source/core/engine.cpp
        source/core/engineInit.cpp
        source/core/draw.cpp
        source/core/asset.cpp
        source/core/event_manager.cpp

        source/core/ui_renderer_begin.cpp
        source/core/ui_renderer_draw.cpp
        source/core/ui_renderer_draw_state.cpp


        source/resource/descriptorPool.cpp
        source/resource/shader_module.cpp
        source/resource/buffer.cpp
        source/model/texture.cpp
        source/model/camera.cpp
        source/resource/swapchain.cpp


        extern/imgui/imgui_impl_glfw.cpp
        extern/imgui/imgui.cpp
        extern/imgui/imgui_draw.cpp
        extern/imgui/imgui_demo.cpp
        extern/imgui/imgui_tables.cpp
        extern/imgui/imgui_widgets.cpp
        extern/imgui/back/imgui_impl_vulkan.cpp
)
find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)


target_link_libraries(exe
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
