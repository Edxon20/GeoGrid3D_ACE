cmake_minimum_required(VERSION 3.20)
project(GeoGrid3D_Lexer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ruta a las cabeceras de la biblioteca DeSiGNAR
set(DESIGNAR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../DeSiGNAR/include")

if (NOT EXISTS "${DESIGNAR_INCLUDE_DIR}/lexer.hpp")
    message(FATAL_ERROR "No se encontro la biblioteca DeSiGNAR en: ${DESIGNAR_INCLUDE_DIR}")
endif()

include_directories(
    ${DESIGNAR_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Ejecutable del Analizador Lexico 
add_executable(geogrid_lexer
    src/main.cpp
)

if (MSVC)
    target_compile_options(geogrid_lexer PRIVATE /W4 /EHsc)
else()
    target_compile_options(geogrid_lexer PRIVATE -Wall -Wextra -pedantic)
endif()
