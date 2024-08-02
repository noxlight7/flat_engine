include_directories(${CMAKE_CURRENT_LIST_DIR})

include_directories("stb/stb_image.h")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/glm)

if(WIN32)
	file(GLOB_RECURSE lib_files_glfw ${CMAKE_CURRENT_LIST_DIR}/glfw/lib/*.lib)
	file(GLOB_RECURSE dll_files_glfw ${CMAKE_CURRENT_LIST_DIR}/glfw/lib/*.dll)

	file(GLOB_RECURSE lib_files_opengl ${CMAKE_CURRENT_LIST_DIR}/opengl/lib/*.lib)
	file(GLOB_RECURSE dll_files_opengl ${CMAKE_CURRENT_LIST_DIR}/opengl/lib/*.dll)

	file(GLOB_RECURSE src_files_glfw
			${CMAKE_CURRENT_LIST_DIR}/glfw/src/*.[ch]pp
			${CMAKE_CURRENT_LIST_DIR}/glfw/src/*.[ch]
	)

	file(GLOB_RECURSE src_files_opengl
			${CMAKE_CURRENT_LIST_DIR}/opengl/src/*.[ch]pp
			${CMAKE_CURRENT_LIST_DIR}/opengl/src/*.[ch]
	)

	include_directories(
			${src_files_glfw}
			${src_files_opengl}
	)

	set(DEPENDENCY_LIBS
			${lib_files_glfw}
			${lib_files_opengl}
	)

else()
	find_package(PkgConfig REQUIRED)

	pkg_check_modules(GLFW REQUIRED glfw3)
	if(NOT GLFW_FOUND)
		message(FATAL_ERROR "GLFW3 not found. Please install it with 'sudo apt-get install libglfw3-dev'")
	endif()

	pkg_search_module(GLFW REQUIRED glfw3)
	pkg_search_module(GLEW REQUIRED glew)

	include_directories(
			${GLFW_INCLUDE_DIRS}
			${GLEW_INCLUDE_DIRS}
	)

	set(DEPENDENCY_LIBS
			${GLFW_LIBRARIES}
			${GLEW_LIBRARIES}
	)
endif()