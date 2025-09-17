cmake_minimum_required(VERSION 3.10)
project(MyProject CXX)
project(MyProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)
set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/extern)
set(MODEL_DIR  ${CMAKE_SOURCE_DIR}/extern/externModel/assets/models/)
set(TEXTURE_DIR  ${CMAKE_SOURCE_DIR}/extern/externModel/assets/textures)

set(KTX ${EXTERN_DIR}/ktx/lib)
include_directories(${CMAKE_SOURCE_DIR}/source/resource)
include_directories(${CMAKE_SOURCE_DIR}/source/renderer)
include_directories(${CMAKE_SOURCE_DIR}/source/model)
include_directories(${CMAKE_SOURCE_DIR}/source/core)

include_directories(${CMAKE_SOURCE_DIR}/extern)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui)

include_directories(${CMAKE_SOURCE_DIR}/extern/ktx/lib)
include_directories(${CMAKE_SOURCE_DIR}/extern/ktx/include)
include_directories(${CMAKE_SOURCE_DIR}/extern/ktx/other_include)


include_directories(${CMAKE_SOURCE_DIR}/extern/imgui/back)
include_directories(${CMAKE_SOURCE_DIR}/extern/imgui/back)

find_package(assimp REQUIRED)
find_package(spdlog REQUIRED)
find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SHADERC_PC REQUIRED shaderc)

add_definitions(-DKTX_USE_KHRONOS_API_VULKAN=1)
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
add_executable(exe
        main.cpp

        source/core/engine.cpp
        source/core/engine_init.cpp
        source/core/engine_vk_load.cpp

        source/core/resource_manager.cpp
        source/core/resource_manager_init.cpp
        source/core/event_manager.cpp

        source/core/renderer.cpp
        source/core/renderer_draw.cpp


        source/core/ui_renderer_begin.cpp
        source/core/ui_renderer_draw.cpp
        source/core/ui_renderer_draw_state.cpp

        source/resource/descriptor_manager.cpp
        source/resource/descriptor_pool.cpp
        source/resource/shader_module.cpp
        source/resource/frame_pool.cpp
        source/resource/memory_pool.cpp
        source/resource/memory_allocator.cpp
        source/resource/static_buffer.cpp
        source/resource/descriptor_uploader.cpp
        source/resource/buffer_builder.cpp
        source/resource/swapchain_view.cpp

        source/model/texture.cpp
        source/model/camera.cpp
        source/model/importer.cpp
        source/model/mesh.cpp

        source/resource/swapchain.cpp
        source/resource/semaphore_pool.cpp


        extern/imgui/imgui_impl_glfw.cpp
        extern/imgui/imgui.cpp
        extern/imgui/imgui_draw.cpp
        extern/imgui/imgui_demo.cpp
        extern/imgui/imgui_tables.cpp
        extern/imgui/imgui_widgets.cpp
        extern/imgui/back/imgui_impl_vulkan.cpp

        source/resource/renderpass_pool.cpp
        source/resource/command_pool.cpp
        source/resource/semaphore_pool.cpp
        source/resource/fence_pool.cpp
        source/resource/sampler_builder.cpp

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
        ktx
)

target_compile_definitions(exe PRIVATE
    ASSET_MODELS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/extern/externModel/assets/models/\"
    ASSET_TEXTURES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/extern/externModel/assets/textures/\"
)
