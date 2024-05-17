cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME cli)
project(${PROJECT_NAME} VERSION 0.1.0)

set(CMAKE_C_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

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

# Build the lib
set(LIBRARY_NAME cli)

add_library(${LIBRARY_NAME} STATIC cli.c)
target_include_directories(${LIBRARY_NAME} PUBLIC "${CMAKE_SOURCE_DIR}")
target_compile_options(${LIBRARY_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

# use CTest and build the test suite
enable_testing()

add_executable(${LIBRARY_NAME}_tests
    test_public.cpp
)

target_link_libraries(${LIBRARY_NAME}_tests
    PRIVATE
    gtest_main
    ${LIBRARY_NAME}
)

target_include_directories(${LIBRARY_NAME}_tests PRIVATE "${CMAKE_SOURCE_DIR}")

target_compile_options(${LIBRARY_NAME}_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)

include(GoogleTest)
gtest_discover_tests(${LIBRARY_NAME}_tests)
