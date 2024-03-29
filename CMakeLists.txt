# cmake_minimum_required(VERSION 3.20.5)
# set(CMAKE_CXX_STANDARD 20)

# project(renderer)

# find_package(OpenGL REQUIRED)
# find_package(Freetype REQUIRED)
# find_package(glfw3 REQUIRED)
# find_package(GLEW REQUIRED)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)



# set(
# OPENGL_SOURCES
#     Vbe/VoxelRenderer/VoxelRenderer.cpp
#     Vbe/VoxelRenderer/Camera/Camera.cpp
#     Vbe/VoxelRenderer/Chunk/Chunk.cpp
#     Vbe/TextRenderer/TextRenderer.cpp
#     Vbe/Engine/Engine.cpp
# )

# set(
# GPP_OPTIONS
#     -Wall
#     -Wextra
# )


# foreach(FLAG ${GPP_OPTIONS})
#     message(${FLAG})
# endforeach()


# add_executable(renderer ${OPENGL_SOURCES})

# target_compile_options(renderer PRIVATE ${GPP_OPTIONS})



# target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
# target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
# target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera)

# target_link_libraries(renderer PRIVATE ${OPENGL_LIBRARIES})
# target_link_libraries(renderer PRIVATE ${FREETYPE_LIBRARIES})
# target_link_libraries(renderer PRIVATE ${GLFW_LIBRARIES})
# target_link_libraries(renderer PRIVATE ${GLEW_LIBRARIES})

# target_link_libraries(renderer PRIVATE glfw)
# include_directories(/usr/include/freetype2)




cmake_minimum_required(VERSION 3.20.5)
project(renderer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wno-unused-command-line-argument)

add_subdirectory(Mist)
add_subdirectory(src)
