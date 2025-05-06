cmake_minimum_required(VERSION 3.25)

if(WIN32)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Путь к vcpkg toolchain")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
get_filename_component(solution_name ${CMAKE_CURRENT_LIST_DIR} NAME)
project(${solution_name})



add_subdirectory(engine)
add_subdirectory(game)

#if(WIN32)
#	file(GLOB_RECURSE dll_files_glfw engine/dependencies/glfw/lib/*.dll)
#	file(GLOB_RECURSE dll_files_opengl engine/dependencies/opengl/lib/*.dll)
#	add_custom_target(dlls_glfw COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_glfw} ${CMAKE_BINARY_DIR})
#	add_dependencies(${solution_name} dlls_glfw)
#	add_custom_target(dlls_opengl COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_opengl} ${CMAKE_BINARY_DIR})
#	add_dependencies(${solution_name} dlls_opengl)
#endif()
