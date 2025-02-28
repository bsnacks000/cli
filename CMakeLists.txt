cmake_minimum_required(VERSION 3.26)

if(CMAKE_VERSION VERSION_LESS "3.26")
    message(FATAL_ERROR "CMake version 3.26 or higher is required!")
endif()

set(PROJECT_NAME cli)
project(${PROJECT_NAME} VERSION 0.1.1)

# standard C17 only
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

option(CLI_BUILD_TESTS "Build the Tests." OFF)
option(CLI_BUILD_EXAMPLES "Build the examples." OFF)

# Add common compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

# Build the lib
set(LIBRARY_NAME cli)
add_library(${LIBRARY_NAME} STATIC cli.c)
target_include_directories(${LIBRARY_NAME} PUBLIC "${CMAKE_SOURCE_DIR}")

if(CLI_BUILD_TESTS)
    # use CTest and build the test suite
    # Add Googletest
    include(cmake/cpm.cmake)
    CPMAddPackage(
        NAME GoogleTest
        GIT_TAG v1.14.0
        VERSION v1.14.0
        GITHUB_REPOSITORY google/googletest
        SOURCE_DIR ${LIB_DIR}/gtest
    )

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

    include(GoogleTest)
    gtest_discover_tests(${LIBRARY_NAME}_tests)
endif()

if(CLI_BUILD_EXAMPLES)
    add_executable(calc examples/example.c)
    target_link_libraries(calc PRIVATE ${LIBRARY_NAME})
endif()
