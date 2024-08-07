cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE game_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[tch]pp)

project(server)

add_executable(server
        ${game_files}
)

target_link_libraries(server PUBLIC engine)
target_link_libraries(server PUBLIC shared)

add_custom_target(copy_assets_server
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_BINARY_DIR}/game/server/assets
)
add_dependencies(server copy_assets_server)

