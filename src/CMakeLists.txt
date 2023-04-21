cmake_minimum_required(VERSION 3.15)
project(LIBBAST)


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

set_target_properties(BAST PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
)

