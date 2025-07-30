
cmake_minimum_required(VERSION 3.10)
project(MyProject CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0") #-O2컴파일러의 최적화 레벨을 2로 설정
include_directories(${CMAKE_SOURCE_DIR}/resource)
include_directories(${CMAKE_SOURCE_DIR}/renderer)
include_directories(${CMAKE_SOURCE_DIR}/extern)
include_directories(${CMAKE_SOURCE_DIR}/model)
include_directories(${CMAKE_SOURCE_DIR}/imgui)
include_directories(${CMAKE_SOURCE_DIR}/imgui/back)
include_directories(${CMAKE_SOURCE_DIR}/core)
add_executable(exe
    main.cpp
    core/Engine.cpp
    core/engineInit.cpp
    core/draw.cpp
    core/asset.cpp
    core/interaction.cpp
    core/uiDraw.cpp
    resource/descriptorPool.cpp
    resource/buffer.cpp
    model/texture.cpp
    model/camera.cpp    
    resource/swapchain.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp 
    imgui/back/imgui_impl_vulkan.cpp
)
#dependency
find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)


# 1. 매크로 변수로 glslc 경로나 옵션 지정
set(GLSLC_EXEC "/usr/bin/glslc")
set(GLSLC_FLAGS      "-O --target-env=vulkan1.2")

set(VERT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/shader/vertex.vert)
set(VERT_OUT ${CMAKE_CURRENT_BINARY_DIR}/shader/vertex.spv)

set(FRAG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/shader/fragment.frag)
set(FRAG_OUT ${CMAKE_CURRENT_BINARY_DIR}/shader/fragment.spv)


add_custom_command(
    OUTPUT ${FRAG_OUT}
    COMMAND ${GLSLC_EXEC} ${FRAG_SRC} -o ${FRAG_OUT}
    DEPENDS ${FRAG_SRC}
)

add_custom_command(
    OUTPUT ${VERT_OUT}
    COMMAND ${GLSLC_EXEC} ${VERT_SRC} -o ${VERT_OUT}
    DEPENDS ${VERT_SRC}
)

add_custom_target(compile_shaders ALL DEPENDS ${VERT_OUT} ${FRAG_OUT})



#library link
#same with
#LDFLAGS = 
#-lglfw 
#-lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
# main: glfw else dependency for glfw 
# conda deactivate :mv ~/anaconda3/lib/libstdc++.so.6 ~/anaconda3/lib/libstdc++.so.6.bak

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
)
