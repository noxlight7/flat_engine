cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE shared_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.[tch]pp)

project(shared)

set(FLATBUFFERS_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated_fbs")
# --- Пусть схемы лежат в папке schemas ---
set(FLATBUFFERS_SCHEMA_DIR "schemas")

# --- Ищем все .fbs ---
file(GLOB FLATBUFFERS_SCHEMAS "${FLATBUFFERS_SCHEMA_DIR}/*.fbs")

# --- Выходная папка для сгенерированных .h (во время сборки) ---
#set(FLATBUFFERS_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated_fbs")

# Собираем список генерируемых файлов
set(GENERATED_HEADERS)
foreach(SCHEMA ${FLATBUFFERS_SCHEMAS})
    get_filename_component(SCHEMA_NAME ${SCHEMA} NAME_WE)
    set(GENERATED_HEADER "${FLATBUFFERS_GENERATED_DIR}/${SCHEMA_NAME}_generated.h")

    add_custom_command(
            OUTPUT ${GENERATED_HEADER}
            COMMAND flatc
            --cpp
            -o "${FLATBUFFERS_GENERATED_DIR}"
            "${SCHEMA}"
            DEPENDS ${SCHEMA}
            COMMENT "Generating FlatBuffers header from ${SCHEMA}"
    )
    list(APPEND GENERATED_HEADERS ${GENERATED_HEADER})
endforeach()

# Создаём цель для генерации .h
add_custom_target(generate_flatbuffers ALL DEPENDS ${GENERATED_HEADERS})

# Метим их как GENERATED
set_source_files_properties(${GENERATED_HEADERS} PROPERTIES GENERATED TRUE)


add_library(shared STATIC
        ${shared_files}
)

target_include_directories(shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(shared PUBLIC engine)
