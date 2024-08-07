cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE shared_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[tch]pp)

project(shared)

add_library(shared
        ${shared_files}
)

target_link_libraries(shared PUBLIC engine)
