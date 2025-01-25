cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим Boost
find_package(Boost CONFIG REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install it with 'sudo apt-get install libboost-all-dev'")
endif()

file(GLOB_RECURSE game_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[tch]pp)

project(client)

add_executable(client
    ${game_files}
)

target_link_libraries(client PUBLIC 
    engine
    shared
    ${Boost_LIBRARIES}
)

target_include_directories(client PUBLIC
    ${Boost_INCLUDE_DIRS}
)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_BINARY_DIR}/game/client/assets
)
add_dependencies(client copy_assets)
