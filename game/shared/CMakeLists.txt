cmake_minimum_required(VERSION 3.25)
project(shared)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Пути и файлы ---
file(GLOB_RECURSE shared_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.[tch]pp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[tch]pp
)

set(FLATBUFFERS_SCHEMA_DIR "schemas")
file(GLOB FLATBUFFERS_SCHEMAS
    "${FLATBUFFERS_SCHEMA_DIR}/*.fbs"
)
set(FLATBUFFERS_GENERATED_DIR
    "${CMAKE_BINARY_DIR}/generated_fbs"
)

# --- 1. Поиск flatc на платформах ---
if (WIN32)
  find_program(FLATBUFFERS_FLATC_EXECUTABLE
    NAMES flatc.exe
    HINTS $ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/tools/flatbuffers
  )
elseif (UNIX AND NOT APPLE)
  find_program(FLATBUFFERS_FLATC_EXECUTABLE
    NAMES flatc
  )
endif()

# --- 2. Создание папки для выходных файлов ---
file(MAKE_DIRECTORY "${FLATBUFFERS_GENERATED_DIR}")

# --- 3. Генерация заголовков ---
set(GENERATED_HEADERS)
foreach(SCHEMA IN LISTS FLATBUFFERS_SCHEMAS)
  get_filename_component(SCHEMA_NAME ${SCHEMA} NAME_WE)
  set(GENERATED_HEADER
      "${FLATBUFFERS_GENERATED_DIR}/${SCHEMA_NAME}_generated.h")

  add_custom_command(
    OUTPUT ${GENERATED_HEADER}
    COMMAND "${FLATBUFFERS_FLATC_EXECUTABLE}"
    --cpp
    -o "${FLATBUFFERS_GENERATED_DIR}"
    "${SCHEMA}"
    DEPENDS ${SCHEMA}
    COMMENT "Generating FlatBuffers header from ${SCHEMA}"
  )
  list(APPEND GENERATED_HEADERS ${GENERATED_HEADER})
endforeach()

add_custom_target(generate_flatbuffers ALL
  DEPENDS ${GENERATED_HEADERS}
)

set_source_files_properties(
  ${GENERATED_HEADERS}
  PROPERTIES GENERATED TRUE
)

# --- 4. Собираем библиотеку ---
add_library(shared STATIC ${shared_files})

target_include_directories(shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${FLATBUFFERS_GENERATED_DIR}
)

target_link_libraries(shared PUBLIC engine)
