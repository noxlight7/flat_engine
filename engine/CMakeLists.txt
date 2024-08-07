cmake_minimum_required(VERSION 3.25)

project(engine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_subdirectory(dependencies)
include("dependencies/CMakeLists.txt")

file(GLOB_RECURSE src_files src/*.[tch]pp)

add_library(engine STATIC ${src_files})

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb
)

add_library(dependencies INTERFACE)

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/stb
        ${INCLUDE_LIBS}
)

if(WIN32)
    file(GLOB_RECURSE dll_files_glfw dependencies/glfw/lib/*.dll)
    file(GLOB_RECURSE dll_files_opengl dependencies/opengl/lib/*.dll)
    add_custom_target(dlls_glfw COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_glfw} ${CMAKE_BINARY_DIR}/game/client)
    add_dependencies(engine dlls_glfw)
    add_custom_target(dlls_opengl COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_opengl} ${CMAKE_BINARY_DIR}/game/client)
    add_dependencies(engine dlls_opengl)
endif()

target_link_libraries(engine PUBLIC ${DEPENDENCY_LIBS})
