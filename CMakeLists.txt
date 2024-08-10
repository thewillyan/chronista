cmake_minimum_required(VERSION 3.10)
project(Chronista VERSION 0.1 LANGUAGES CXX)

# set variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)  # For static libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)  # For shared libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # For executables

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()

# include directories
include_directories(include)
include_directories(lib)

# create library "Chronista"
file(GLOB_RECURSE
    CHRONISTA_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/chronista/*.hpp" "${CMAKE_SOURCE_DIR}/src/chronista/*.cpp")
add_library(Chronista ${CHRONISTA_SOURCES})

# add main executable
add_executable(chronista_cli "${CMAKE_SOURCE_DIR}/exe/main.cpp")
target_link_libraries(chronista_cli PUBLIC Chronista)
add_custom_target(run
    COMMAND ./chronista_cli
    DEPENDS chronista_cli
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# link test library
add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE "${CMAKE_SOURCE_DIR}/lib/doctest")

# setup tests
enable_testing()

file(GLOB_RECURSE
    TEST_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/chronista/*.cpp")
add_executable(chronista_tests "${CMAKE_SOURCE_DIR}/tests/main.cpp" "${TEST_FILES}")
target_compile_features(chronista_tests PRIVATE cxx_std_17)
target_link_libraries(chronista_tests PRIVATE doctest Chronista)

add_test(NAME ChronistaTests COMMAND chronista_tests)
