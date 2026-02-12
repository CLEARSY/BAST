include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# Sanitizers for Debug builds (GCC/Clang only)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID MATCHES Clang$)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
  endif()
endif()

function(bast_add_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PRIVATE BAST_LIB tinyxml2 GTest::gtest_main)
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
  gtest_discover_tests(${name})
endfunction()

bast_add_test(test_hash)
bast_add_test(test_btype)
bast_add_test(test_vars)
bast_add_test(test_expr)
bast_add_test(test_pred)
bast_add_test(test_subst)
