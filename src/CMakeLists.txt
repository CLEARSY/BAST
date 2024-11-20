project(BASTLIB)

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
    exprReader.h
    predReader.h
    gpredReader.h
    substReader.h
    exprWriter.h
    predWriter.h
    hash.h
)

set(BAST_SOURCES
    btype.cpp
    vars.cpp
    expr.cpp
    pred.cpp
    gpred.cpp
    exprDesc.cpp
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

add_library(BAST_LIB STATIC ${BAST_SOURCES} ${BAST_HEADERS})
target_link_libraries(BAST_LIB PRIVATE Qt5::Core Qt5::Xml)

target_compile_features(BAST_LIB PUBLIC cxx_std_20)
set_target_properties(BAST_LIB PROPERTIES PREFIX "lib" OUTPUT_NAME "BAST")
