# BAST

A C++ library to represent B Abstract Syntax Trees

This source code is used as 

- a submodule of AtelierB

## Version nomber

There is currently no formal policy to give official identifiers to versions of this code.

## Dependencies

The library depends on the libraries Qt5Core and Qt5Xml. 

## Compilation

Compilation is realized with the [CMake](https://cmake.org) software build system.

The `CMakeLists.txt` file defines the following:

- The target `BAST_LIB` for the static library `libBAST.a`
- The directory `$BAST_SOURCE_DIR` contains the library headers
- The directory `$BAST_SOURCE_DIR` contains the static library

Assuming

- `$SOURCE` is the path of the directory where the git repository has been cloned
- `$BUILD` is the path of the directory where the build is launched

The following commands build the library :

```sh
cd $BUILD
cmake "$SOURCE"
cmake --build .
```

### Instructions for Linux

Package `libqtbase5-dev` (or equivalent) shall have been installed

### Instructions for Windows

The library has been successfully build with `MSYS2-Mingw-w64` using the following package environment :

```sh
pacman -S --needed --noconfirm \
    mingw-w64-ucrt-x86_64-{cmake,ninja,gcc} \
    mingw-w64-ucrt-x86_64-qt5-base \


