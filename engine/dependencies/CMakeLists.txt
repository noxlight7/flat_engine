add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/glm)

## Находим Boost
#find_package(Boost REQUIRED COMPONENTS system)
#if(NOT Boost_FOUND)
#    message(FATAL_ERROR "Boost not found. Please install it with 'sudo apt-get install libboost-all-dev'")
#endif()

if(WIN32)
	file(GLOB_RECURSE lib_files_glfw ${CMAKE_CURRENT_LIST_DIR}/glfw/lib/*.lib)

	file(GLOB_RECURSE lib_files_opengl ${CMAKE_CURRENT_LIST_DIR}/opengl/lib/*.lib)

	file(GLOB_RECURSE src_files_glfw
			${CMAKE_CURRENT_LIST_DIR}/glfw/GLFW/*.[ch]pp
			${CMAKE_CURRENT_LIST_DIR}/glfw/GLFW/*.[ch]
	)

	file(GLOB_RECURSE src_files_opengl
			${CMAKE_CURRENT_LIST_DIR}/opengl/GL/*.[ch]pp
			${CMAKE_CURRENT_LIST_DIR}/opengl/GL/*.[ch]
	)

	set(INCLUDE_LIBS
			${CMAKE_CURRENT_LIST_DIR}/glfw
			${CMAKE_CURRENT_LIST_DIR}/opengl
	)

	set(DEPENDENCY_LIBS
			${lib_files_glfw}
			${lib_files_opengl}
			${glm}
	)

else()
	find_package(PkgConfig REQUIRED)

	pkg_check_modules(GLFW REQUIRED glfw3)
	if(NOT GLFW_FOUND)
		message(FATAL_ERROR "GLFW3 not found. Please install it with 'sudo apt-get install libglfw3-dev'")
	endif()

	pkg_search_module(GLFW REQUIRED glfw3)
	pkg_search_module(GLEW REQUIRED glew)

	set(DEPENDENCY_LIBS
			${GLFW_LIBRARIES}
			${GLEW_LIBRARIES}
			${glm}
			${Boost_LIBRARIES}  # Добавляем библиотеки Boost
	)

	set(INCLUDE_LIBS
			${GLFW_INCLUDE_DIRS}
			${GLEW_INCLUDE_DIRS}
			${Boost_INCLUDE_DIRS}  # Добавляем include-директории Boost
	)
endif()
