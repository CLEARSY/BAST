cmake_minimum_required(VERSION 3.15)
project(pogPrinter
    VERSION 1.0
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(pogPrinterConfig.h.in pogPrinterConfig.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${BASTLIB_SOURCE_DIR})

add_executable(pogPrinter pogPrinter.cpp)
target_link_libraries(pogPrinter  -Bstatic BAST_LIB tinyxml2::tinyxml2)
