cmake_minimum_required(VERSION 3.28)

project(
    c_surfer
    VERSION 0.1.0
    LANGUAGES CXX
)

# ------------------------------------------------------------------------------
# Global options
# ------------------------------------------------------------------------------
option(ENABLE_LOGGING "Enable application logging" ON)
set(LOG_LEVEL "INFO" CACHE STRING "Log level: TRACE, DEBUG, INFO, WARN, ERROR")

set_property(CACHE LOG_LEVEL PROPERTY STRINGS TRACE DEBUG INFO WARN ERROR)

# ------------------------------------------------------------------------------
# Compiler settings
# ------------------------------------------------------------------------------
add_library(project_warnings INTERFACE)
target_compile_features(project_warnings INTERFACE cxx_std_23)

if (MSVC)
    target_compile_options(project_warnings INTERFACE /W4 /permissive-)
else()
    target_compile_options(project_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------------------------
# CPM.cmake
# ------------------------------------------------------------------------------
set(CPM_DOWNLOAD_LOCATION
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake"
    CACHE FILEPATH "CPM.cmake location"
)

if (NOT EXISTS "${CPM_DOWNLOAD_LOCATION}")
    message(STATUS "Downloading CPM.cmake...")
    file(DOWNLOAD
        "https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake"
        "${CPM_DOWNLOAD_LOCATION}"
        STATUS status
        SHOW_PROGRESS
    )
    list(GET status 0 status_code)
    if (NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download CPM.cmake")
    endif()
endif()

include(${CPM_DOWNLOAD_LOCATION})

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

# SDL2
CPMAddPackage(
    NAME SDL2
    GITHUB_REPOSITORY libsdl-org/SDL
    GIT_TAG SDL2
    OPTIONS
        "SDL_TEST OFF"
        "SDL_EXAMPLES OFF"
)

# OpenSSL / FreeType
find_package(OpenSSL REQUIRED)
find_package(Freetype REQUIRED)

# ------------------------------------------------------------------------------
# Skia (external build)
# ------------------------------------------------------------------------------
include(ExternalProject)

set(SKIA_DIR "${PROJECT_SOURCE_DIR}/external/skia")
set(SKIA_OUT_DIR "${SKIA_DIR}/out/Static")
set(SKIA_LIB "${SKIA_OUT_DIR}/libskia.a")

# 1. External build target (UTILITY target)
ExternalProject_Add(
    skia_ep
    GIT_REPOSITORY https://skia.googlesource.com/skia.git
    GIT_TAG chrome/m144

    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/skia
    BUILD_IN_SOURCE 1

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${PROJECT_SOURCE_DIR}/scripts/build_skia.sh
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${SKIA_LIB}
)

# 2. Interface library consumers will link against
add_library(skia INTERFACE)

# Make sure Skia is built before anything that links it
add_dependencies(skia skia_ep)

# SYSTEM for skia warnings
target_include_directories(skia SYSTEM INTERFACE
    ${SKIA_DIR}/include
    ${SKIA_DIR}/include/config
    ${SKIA_DIR}
)

target_link_libraries(skia INTERFACE
    ${SKIA_LIB}
    pthread
    dl
    m
)

# ------------------------------------------------------------------------------
# Main executable
# ------------------------------------------------------------------------------
add_executable(c_surfer
    src/main.cpp
    src/browser/Browser.cpp
    src/url/Url.cpp
    src/request/HttpRequest.cpp
)

# Add include directories for src/
target_include_directories(c_surfer PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(c_surfer PRIVATE
    project_warnings
    skia
    SDL2::SDL2
    OpenSSL::SSL
    OpenSSL::Crypto
    Freetype::Freetype
)

# ------------------------------------------------------------------------------
# Logging configuration (compile-time)
# ------------------------------------------------------------------------------
if (ENABLE_LOGGING)
    target_compile_definitions(c_surfer PRIVATE
        ENABLE_LOGGING=1
        LOG_LEVEL_${LOG_LEVEL}=1
    )
    message(STATUS "Logging enabled (level: ${LOG_LEVEL})")
else()
    target_compile_definitions(c_surfer PRIVATE ENABLE_LOGGING=0)
    message(STATUS "Logging disabled")
endif()
