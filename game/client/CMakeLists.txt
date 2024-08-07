cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE game_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[tch]pp)

project(client)

add_executable(client
        ${game_files}
)

target_link_libraries(client PUBLIC engine)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_BINARY_DIR}/game/client/assets
)
add_dependencies(client copy_assets)

