cmake_minimum_required(VERSION 3.25)

project(ogf-network LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Поиск Boost (system) ---
find_package(Boost CONFIG REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif()

# --- Поиск FlatBuffers ---
find_package(flatbuffers REQUIRED)
if(NOT flatbuffers_FOUND)
    message(FATAL_ERROR "FlatBuffers is not found")
endif()

set(FLATBUFFERS_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated_fbs")

# --- Собираем список cpp/h для фреймворка (shared, server, client) ---
file(GLOB_RECURSE NETWORK_SOURCES_SHARED
        "src/shared/*.cpp"
)
file(GLOB_RECURSE NETWORK_SOURCES_SERVER
        "src/server/*.cpp"
)
file(GLOB_RECURSE NETWORK_SOURCES_CLIENT
        "src/client/*.cpp"
)

# Можно объединить все .cpp в один список
set(OGF_NETWORK_SOURCES
        ${NETWORK_SOURCES_SHARED}
        ${NETWORK_SOURCES_SERVER}
        ${NETWORK_SOURCES_CLIENT}
        # .cpp, если генерируете FlatBuffers в .cpp (по умолчанию .h)
        # ${GENERATED_SOURCES}
)

# Создаём статическую библиотеку (можно STATIC или SHARED)
add_library(ogf-network SHARED
        ${OGF_NETWORK_SOURCES}
        ${GENERATED_HEADERS}  # Чтобы они были видны на этапе компиляции (хотя .h)
)

# Зависимость от generate_flatbuffers
add_dependencies(ogf-network generate_flatbuffers)

# Подключаем нужные директории
target_include_directories(ogf-network PUBLIC
        # Папка, где лежат *.h фреймворка (shared, server, client):
        # Например, если вы хотите #include "shared/transport.h" -- можно прописать так:
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/shared>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/server>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/client>

        # Папка для сгенерированных .h
        $<BUILD_INTERFACE:${FLATBUFFERS_GENERATED_DIR}>

        # Boost
        ${Boost_INCLUDE_DIRS}
)

# Линкуем flatbuffers, Boost
target_link_libraries(ogf-network
        PUBLIC
        Boost::system
        flatbuffers::flatbuffers
)

# Можно установить свойства для C++17/20/23, warning и т.д. при желании
