cmake_minimum_required(VERSION 3.15)
project(pogPrinter 
    VERSION 1.0
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(pogPrinterConfig.h.in pogPrinterConfig.h)

add_executable(pogPrinter
    pogPrinter.cpp
)

target_include_directories(pogPrinter
    PUBLIC "${PROJECT_BINARY_DIR}" "${BAST_SOURCE_DIR}"
)

target_link_libraries(pogPrinter
    PUBLIC BAST
)
