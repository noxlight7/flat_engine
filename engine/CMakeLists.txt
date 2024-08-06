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

target_link_libraries(engine PUBLIC ${DEPENDENCY_LIBS})
