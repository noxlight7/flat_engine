cmake_minimum_required(VERSION 3.2)

file (GLOB_RECURSE src_files ${CMAKE_CURRENT_LIST_DIR}/src/*.[ch]pp)

file (GLOB_RECURSE game_files ${CMAKE_CURRENT_LIST_DIR}/game/*.[ch]pp)

get_filename_component (solution_name ${CMAKE_CURRENT_LIST_DIR} NAME)
project (${solution_name})

include_directories ("src" "game")

include ("dependencies/CMakeLists.txt")
add_executable ( ${solution_name} 
	${src_files}
	${src_files_glfw} 
	${src_files_vulcan} 
	${src_files_opengl} 
	${src_files_stb} 
	${game_files}
 )

target_link_libraries ( ${solution_name} 
	${lib_files_glfw} 
	${lib_files_vulcan} 
	${lib_files_opengl}
)

add_custom_target (dlls_glfw COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_glfw} ${CMAKE_BINARY_DIR})
add_dependencies (${solution_name} dlls_glfw)

# TODO: add opengl/vulcan dependencies.
#add_custom_target (dlls_vulcan COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_vulcan} ${CMAKE_BINARY_DIR})
#add_dependencies (${solution_name} dlls_vulcan)

add_custom_target (dlls_opengl COMMAND ${CMAKE_COMMAND} -E copy ${dll_files_opengl} ${CMAKE_BINARY_DIR})
add_dependencies (${solution_name} dlls_opengl)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_BINARY_DIR}/assets
)
add_dependencies (${solution_name} copy_assets)

set_property (TARGET ${solution_name} PROPERTY CXX_STANDARD 20)