cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE game_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[tch]pp)

project(client)

add_executable(client
        ${game_files}
)

target_link_libraries(client PUBLIC engine)

#if(WIN32)
#    file(GLOB_RECURSE dll_files_glfw engine/dependencies/glfw/lib/*.dll)
#    file(GLOB_RECURSE dll_files_opengl engine/dependencies/opengl/lib/*.dll)
#    add_custom_target(dlls_glfw COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_glfw} ${CMAKE_BINARY_DIR})
#    add_dependencies(${solution_name} dlls_glfw)
#    add_custom_target(dlls_opengl COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_opengl} ${CMAKE_BINARY_DIR})
#    add_dependencies(${solution_name} dlls_opengl)
#endif()

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_BINARY_DIR}/game/client/assets
)
add_dependencies(client copy_assets)

