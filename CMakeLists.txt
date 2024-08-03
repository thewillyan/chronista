cmake_minimum_required(VERSION 3.10)
project(Chronista VERSION 0.1 LANGUAGES CXX)

# set variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # For static libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # For shared libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # For executables

# include directories
include_directories(include)

# create library "Chronista"
file(GLOB_RECURSE
    CHRONISTA_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/chronista/*.hpp" "${CMAKE_SOURCE_DIR}/src/chronista/*.cpp")
add_library(Chronista ${CHRONISTA_SOURCES})

add_executable(chronista_cli "${CMAKE_SOURCE_DIR}/exe/main.cpp")
target_link_libraries(chronista_cli PUBLIC Chronista)
add_custom_target(run
    COMMAND ./chronista_cli
    DEPENDS chronista_cli
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# tests
enable_testing()
find_package(doctest REQUIRED)

file(GLOB_RECURSE
    TEST_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/chronista/*.cpp")
add_executable(chronista_tests "${CMAKE_SOURCE_DIR}/tests/main.cpp" "${TEST_FILES}")
target_compile_features(chronista_tests PRIVATE cxx_std_17)
target_link_libraries(chronista_tests PRIVATE doctest::doctest Chronista)

add_test(NAME ChronistaTests COMMAND chronista_tests)
