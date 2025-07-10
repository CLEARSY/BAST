cmake_minimum_required(VERSION 3.15)
project(LIBBAST)

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # additional warnings
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

set(BAST_HEADERS
    btype.h
    vars.h
    expr.h
    pred.h
    gpred.h
    exprDesc.h
    predDesc.h
    subst.h
    btypeReader.h
    exprReader.h
    predReader.h
    gpredReader.h
    substReader.h
    exprWriter.h
    predWriter.h
    btypeXmlWriter.h
    btypePrinter.h
    exprPrinter.h
    predPrinter.h
    hash.h
)

set(BAST_SRC
    btype.cpp
    vars.cpp
    expr.cpp
    pred.cpp
    gpred.cpp
    subst.cpp
    btypeReader.cpp
    exprReader.cpp
    predReader.cpp
    predPrinter.h
    gpredReader.cpp
    substReader.cpp
    exprWriter.cpp
    predWriter.cpp
    btypeXmlWriter.cpp
    btypePrinter.cpp
    exprPrinter.cpp
    predPrinter.cpp
)

set(TINYXML2
  tinyxml2.cpp tinyxml2.h
)
include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${BAST_SOURCE_DIR}
)

add_library(BAST
    ${BAST_SRC}
    ${BAST_HEADERS}
    ${TINYXML2}
)

target_compile_features(BAST PUBLIC cxx_std_17)
set_target_properties(BAST PROPERTIES PREFIX "lib" OUTPUT_NAME "BAST")
