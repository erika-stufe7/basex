cmake_minimum_required(VERSION 3.12)
project(basex VERSION 0.9.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build options
option(DISABLE_SIMD "Disable SIMD optimizations" OFF)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")

# Detect CPU features
include(CheckCCompilerFlag)
include(CheckCSourceRuns)

if(NOT DISABLE_SIMD)
    # Check for AVX2 support
    check_c_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
    if(COMPILER_SUPPORTS_AVX2)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")
        add_definitions(-DHAVE_AVX2)
    endif()

    # Check for BMI1/BMI2 support
    check_c_compiler_flag("-mbmi -mbmi2" COMPILER_SUPPORTS_BMI)
    if(COMPILER_SUPPORTS_BMI)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mbmi -mbmi2")
        add_definitions(-DHAVE_BMI)
    endif()

    # Check for SSE4.2 support
    check_c_compiler_flag("-msse4.2" COMPILER_SUPPORTS_SSE42)
    if(COMPILER_SUPPORTS_SSE42)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.2")
        add_definitions(-DHAVE_SSE42)
    endif()
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Shared library - libbasex
add_library(basex SHARED
    src/libbasex/base85.c
    src/libbasex/base91.c
    src/libbasex/base122.c
    src/libbasex/cpu_detect.c
    src/libbasex/common.c
)

if(NOT DISABLE_SIMD)
    target_sources(basex PRIVATE
        src/libbasex/simd/base85_avx2.c
        src/libbasex/simd/base91_avx2.c
        src/libbasex/simd/base122_avx2.c
    )
endif()

set_target_properties(basex PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/basex.h
)

# CLI executables
add_executable(base85 src/cli/base85_cli.c)
target_link_libraries(base85 basex)

add_executable(base91 src/cli/base91_cli.c)
target_link_libraries(base91 basex)

add_executable(base122 src/cli/base122_cli.c)
target_link_libraries(base122 basex)

# zstd + base encoding executables
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZSTD REQUIRED libzstd)

add_executable(zbase85 src/cli/zbase85_cli.c)
target_link_libraries(zbase85 basex ${ZSTD_LIBRARIES})
target_include_directories(zbase85 PRIVATE ${ZSTD_INCLUDE_DIRS})

add_executable(zbase91 src/cli/zbase91_cli.c)
target_link_libraries(zbase91 basex ${ZSTD_LIBRARIES})
target_include_directories(zbase91 PRIVATE ${ZSTD_INCLUDE_DIRS})

add_executable(zbase122 src/cli/zbase122_cli.c)
target_link_libraries(zbase122 basex ${ZSTD_LIBRARIES})
target_include_directories(zbase122 PRIVATE ${ZSTD_INCLUDE_DIRS})

# Tests
if(BUILD_TESTS AND EXISTS ${CMAKE_SOURCE_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
endif()

# Benchmarks
if(BUILD_BENCHMARKS AND EXISTS ${CMAKE_SOURCE_DIR}/benchmarks)
    add_subdirectory(benchmarks)
endif()

# Installation
install(TARGETS basex base85 base91 base122 zbase85 zbase91 zbase122
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

install(FILES
    man/base85.1
    man/base91.1
    man/base122.1
    man/zbase85.1
    man/zbase91.1
    man/zbase122.1
    DESTINATION share/man/man1
)

# CPack configuration for Debian package
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "basex")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "High-performance base encoding tools")
set(CPACK_PACKAGE_DESCRIPTION "Fast base85, base91, and base122 encoding tools with CPU optimizations")
set(CPACK_PACKAGE_CONTACT "your.email@example.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.31)")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_amd64")

include(CPack)
