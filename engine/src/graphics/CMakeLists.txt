cmake_minimum_required(VERSION 3.25)

project(flat_graphics)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(res/opengl)
add_subdirectory(dependencies/glm/glm)

# Библиотека графики
add_library(flat_graphics STATIC
        dependencies/glad/src/glad.c
        include/renderer.hpp
        include/opengl/opengl_renderer.hpp
        src/opengl/opengl_renderer.cpp
        src/renderer.cpp
        include/opengl/opengl_texture_manager.hpp
        src/opengl/opengl_texture_manager.cpp
        include/opengl/opengl_vertex_buffer.hpp
        src/opengl/opengl_vertex_buffer.cpp
        include/opengl/opengl_graphics_constants.hpp
        include/opengl/opengl_shader.hpp
        src/opengl/opengl_shader.cpp
        include/texture_manager.hpp
        include/texture.hpp
        include/opengl/opengl_texture.hpp
        src/opengl/opengl_texture.cpp
        include/camera.hpp
)

# Заголовочные файлы
target_include_directories(flat_graphics PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb_image
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_include_directories(flat_graphics PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm
)

# Линковка GLFW
target_link_libraries(flat_graphics PUBLIC glfw)

# Для Linux: дополнительные зависимости
if (UNIX AND NOT APPLE)
    target_link_libraries(flat_graphics PUBLIC
            X11
            pthread
            dl
            GL
    )
endif()