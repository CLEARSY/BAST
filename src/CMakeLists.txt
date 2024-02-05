project(BASTLIB)

set(BAST_HEADERS
    btype.h
    vars.h
    expr.h
    pred.h
    gpred.h
    exprDesc.h
    predDesc.h
    subst.h
    exprReader.h
    predReader.h
    gpredReader.h
    substReader.h
    exprWriter.h
    predWriter.h
    hash.h
)

set(BAST_SRC
    btype.cpp
    vars.cpp
    expr.cpp
    pred.cpp
    gpred.cpp
    subst.cpp
    exprReader.cpp
    predReader.cpp
    gpredReader.cpp
    substReader.cpp
    exprWriter.cpp
    predWriter.cpp
)

find_package(Qt5 REQUIRED COMPONENTS Core Xml)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${BAST_SOURCE_DIR}
)

add_library(BAST
    ${BAST_SRC}
    ${BAST_HEADERS}
)

target_link_libraries(BAST PRIVATE Qt5::Core Qt5::Xml)
