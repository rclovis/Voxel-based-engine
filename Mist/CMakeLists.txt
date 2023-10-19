cmake_minimum_required(VERSION 3.25)
project(mist)

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_compile_options(-Wno-unused-command-line-argument)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    SOURCES
    VoxelRenderer/VoxelRenderer.cpp
    VoxelRenderer/Camera/Camera.cpp
    VoxelRenderer/Chunk/Chunk.cpp
    TextRenderer/TextRenderer.cpp
    Engine/Engine.cpp
    Engine/Shader.cpp
    Engine/VoxLoader.cpp
)
add_library(mist STATIC ${SOURCES})
target_include_directories(mist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(mist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine)
target_include_directories(mist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/TextRenderer)
target_include_directories(mist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/VoxelRenderer)
target_include_directories(mist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/VoxelRenderer/Camera)
target_include_directories(mist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/VoxelRenderer/Chunk)

target_link_libraries(mist PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(mist PUBLIC ${FREETYPE_LIBRARIES})
target_link_libraries(mist PUBLIC ${GLFW_LIBRARIES})
target_link_libraries(mist PUBLIC ${GLEW_LIBRARIES})

target_link_libraries(mist PUBLIC glfw)
include_directories(/usr/include/freetype2)
