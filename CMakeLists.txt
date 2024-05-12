cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME cli)
project(${PROJECT_NAME} VERSION 0.1.0)

set(CMAKE_C_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_BUILD_TYPE Release)

set(LIBRARY_NAME cli)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Add Googletest
include(cmake/cpm.cmake)
CPMAddPackage(
    NAME GoogleTest
    GIT_TAG v1.14.0
    VERSION v1.14.0
    GITHUB_REPOSITORY google/googletest
    SOURCE_DIR ${LIB_DIR}/gtest
)

# use CTest and build the test suite
enable_testing()

add_executable(${LIBRARY_NAME}_tests
    test.cpp
)

target_link_libraries(${LIBRARY_NAME}_tests
    PRIVATE
    gtest_main
)

target_compile_options(${LIBRARY_NAME}_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)

include(GoogleTest)
gtest_discover_tests(${LIBRARY_NAME}_tests)
