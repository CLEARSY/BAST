# BAST

A C++ library to represent B Abstract Syntax Trees.

## Scope

BAST is a C++ library to represent substitutions, predicates, expressions and types. BAST *does not represent B components*.

BAST does not try to share identical sub-terms.

## Assets

This repository contains the source files for 

1. the BAST library 

1. pogPrinter, a utility to print POG files to a more human-readable format.

## Compiling

Compilation may be done with `cmake` thanks to the `CMakeLists.txt`file available in the distribution. The following commands compile the BAST library and the pogPrinter utility.

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

## Acknowledgments

BAST includes TinyXML-2 source files.

TinyXML-2 is a simple, small, efficient, C++ XML parser that can be easily integrated into other programs.

The master is hosted on github: https://github.com/leethomason/tinyxml2.

TinyXML-2 has its own license, see the above page (also available in this repository under resources/LICENSE-tinyxml2.txt)