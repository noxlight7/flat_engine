cmake_minimum_required(VERSION 3.25)
if(WIN32)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Путь к vcpkg toolchain")
endif()
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

if (WIN32)
    # находит glfw3Config.cmake в vcpkg-пути
   find_package(glfw3 CONFIG REQUIRED)           # подтянет импортированный target glfw :contentReference[oaicite:5]{index=5}
   # линковка с импортированным таргетом
   target_link_libraries(flat_graphics PUBLIC
       glfw                                    # импортированный target :contentReference[oaicite:6]{index=6}
   )
  find_package(OpenGL REQUIRED)            # находит opengl32.lib :contentReference[oaicite:6]{index=6}
  target_link_libraries(flat_graphics PUBLIC
      OpenGL::GL                            # импортированный target :contentReference[oaicite:7]{index=7}
  )
elseif (UNIX AND NOT APPLE)
    target_link_libraries(flat_graphics PUBLIC
            X11
            pthread
            dl
            GL
    )
endif()