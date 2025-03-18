cmake_minimum_required(VERSION 3.25)

project(engine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_subdirectory(dependencies)
include("dependencies/CMakeLists.txt")

add_subdirectory(src/network)
add_subdirectory(src/graphics)

file(GLOB_RECURSE src_files_client src/client/*.[tch]pp)
file(GLOB_RECURSE src_files_server src/server/*.[tch]pp)
file(GLOB_RECURSE src_files_server src/display/*.[tch]pp)
file(GLOB_RECURSE src_files_entities src/entities/*.[tch]pp)
file(GLOB_RECURSE src_files_factory src/factory/*.[tch]pp)
file(GLOB_RECURSE src_files_generators src/generators/*.[tch]pp)
file(GLOB_RECURSE src_files_input src/input/*.[tch]pp)
file(GLOB_RECURSE src_files_physics src/physics/*.[tch]pp)
file(GLOB_RECURSE src_files_texts src/texts/*.[tch]pp)
file(GLOB_RECURSE src_files_utils src/utils/*.[tch]pp)

set(src_files
        ${src_files_client}
        ${src_files_server}
        ${src_files_entities}
        ${src_files_factory}
        ${src_files_generators}
        ${src_files_input}
        ${src_files_physics}
        ${src_files_texts}
        ${src_files_utils}
        ${src_files_display}
        src/entry.hpp
)

add_library(engine STATIC ${src_files})
target_link_libraries(engine PUBLIC ogf-network)

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/dependencies/glm
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
    add_custom_target(dlls_glfw2 COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_glfw} ${CMAKE_BINARY_DIR}/game/server)
    add_dependencies(engine dlls_glfw)
    add_dependencies(engine dlls_glfw2)
    add_custom_target(dlls_opengl COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_opengl} ${CMAKE_BINARY_DIR}/game/client)
    add_custom_target(dlls_opengl2 COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_opengl} ${CMAKE_BINARY_DIR}/game/server)
    add_dependencies(engine dlls_opengl)
    add_dependencies(engine dlls_opengl2)
endif()

target_link_libraries(engine PUBLIC ${DEPENDENCY_LIBS} flat_graphics)
