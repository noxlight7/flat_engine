
include_directories (${CMAKE_CURRENT_LIST_DIR})

file (GLOB_RECURSE lib_files_glfw ${CMAKE_CURRENT_LIST_DIR}/glfw/lib/*.lib)
file (GLOB_RECURSE dll_files_glfw ${CMAKE_CURRENT_LIST_DIR}/glfw/lib/*.dll)

file (GLOB_RECURSE lib_files_vulcan ${CMAKE_CURRENT_LIST_DIR}/vulcan/lib/*.lib)
file (GLOB_RECURSE dll_files_vulcan ${CMAKE_CURRENT_LIST_DIR}/vulcan/lib/*.dll)

file (GLOB_RECURSE lib_files_opengl ${CMAKE_CURRENT_LIST_DIR}/opengl/lib/*.lib)
file (GLOB_RECURSE dll_files_opengl ${CMAKE_CURRENT_LIST_DIR}/opengl/lib/*.dll)

file (GLOB_RECURSE src_files_glfw 
	${CMAKE_CURRENT_LIST_DIR}/glfw/src/*.[ch]pp 
	${CMAKE_CURRENT_LIST_DIR}/glfw/src/*.[ch]
)

file (GLOB_RECURSE src_files_vulcan 
	${CMAKE_CURRENT_LIST_DIR}/vulcan/src/*.[ch]pp 
	${CMAKE_CURRENT_LIST_DIR}/vulcan/src/*.[ch]
)

file (GLOB_RECURSE src_files_opengl 
	${CMAKE_CURRENT_LIST_DIR}/opengl/src/*.[ch]pp 
	${CMAKE_CURRENT_LIST_DIR}/opengl/src/*.[ch]
)

file (GLOB_RECURSE src_files_stb 
	${CMAKE_CURRENT_LIST_DIR}/stb/*.[ch]pp 
	${CMAKE_CURRENT_LIST_DIR}/stb/*.[ch]
)

file (GLOB_RECURSE src_files_slm 
	${CMAKE_CURRENT_LIST_DIR}/slm/*.[ch]pp 
	${CMAKE_CURRENT_LIST_DIR}/slm/*.[ch] 
	${CMAKE_CURRENT_LIST_DIR}/slm/*.inl
)

include_directories (
	${src_files_glfw} 
	${src_files_vulcan} 
	${src_files_opengl} 
	${src_files_stb}
	${src_files_slm}
)