cmake_minimum_required(VERSION 3.28)

project(
    csurfer
    VERSION 0.1.0
    LANGUAGES C CXX
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

CPMAddPackage(
    NAME SDL2_ttf
    GITHUB_REPOSITORY libsdl-org/SDL_ttf
    GIT_TAG release-2.22.0
    OPTIONS
        "SDL2TTF_SAMPLES OFF"
)

# OpenSSL / FreeType
find_package(OpenSSL REQUIRED)
find_package(Freetype REQUIRED)

# Duktape
CPMAddPackage(
    NAME duktape
    URL "https://github.com/svaarala/duktape/releases/download/v2.7.0/duktape-2.7.0.tar.xz"
    DOWNLOAD_ONLY YES
)

if (duktape_ADDED)
    add_library(duktape STATIC
        ${duktape_SOURCE_DIR}/src/duktape.c
    )
    target_include_directories(duktape SYSTEM PUBLIC
        ${duktape_SOURCE_DIR}/src
    )
    # Duktape needs certain flags on some systems, but default should work for now.
endif()

# GoogleTest
CPMAddPackage(
    NAME GTest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG v1.14.0
)

include(ExternalProject)

set(SKIA_DIR "${PROJECT_SOURCE_DIR}/external/skia")
set(SKIA_OUT_DIR "${SKIA_DIR}/out/Static")
set(SKIA_LIB "${SKIA_OUT_DIR}/libskia.a")

add_library(skia INTERFACE)

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
    fontconfig
    freetype
)

# ------------------------------------------------------------------------------
# Core Library
# ------------------------------------------------------------------------------
add_library(csurfer_lib STATIC
    src/dom/Lexeme.cpp
    src/dom/Text.cpp
    src/dom/Element.cpp
    src/html/HTMLParser.cpp
    src/layout/InputLayout.cpp
    src/layout/BlockLayout.cpp
    src/layout/BlockPainter.cpp
    src/layout/InlineLayoutHelper.cpp
    src/layout/DocumentLayout.cpp
    src/layout/DisplayItem.cpp
    src/layout/LineLayout.cpp
    src/layout/TextLayout.cpp
    src/url/Url.cpp
    src/request/HttpRequest.cpp
    src/request/CookieJar.cpp
    src/utils/Parser.cpp
    src/utils/StringUtils.cpp
    src/browser/Browser.cpp
    src/browser/Tab.cpp
    src/browser/TabSecurityPolicy.cpp
    src/browser/TabNavigator.cpp
    src/browser/CSurferUI.cpp
    src/css/CSSSelector.cpp
    src/css/CSSParser.cpp
    src/css/StyleComputation.cpp
    src/css/StyleEngine.cpp
    src/js/JSContext.cpp
    src/gfx/SkiaContext.cpp
    src/gfx/SkiaFont.cpp
    src/gfx/Color.cpp
)

target_include_directories(csurfer_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(csurfer_lib PUBLIC
    project_warnings
    skia
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    OpenSSL::SSL
    OpenSSL::Crypto
    Freetype::Freetype
    duktape
)

# ------------------------------------------------------------------------------
# Main executable
# ------------------------------------------------------------------------------
add_executable(csurfer
    src/main.cpp
)

target_link_libraries(csurfer PRIVATE
    csurfer_lib
)

# ------------------------------------------------------------------------------
# Assets configuration(compile-time)
# ------------------------------------------------------------------------------

# Assuming your assets folder is at project-root/assets
target_compile_definitions(csurfer_lib PUBLIC
    ASSETS_DIR="${PROJECT_SOURCE_DIR}/assets"
)

# ------------------------------------------------------------------------------
# Logging configuration (compile-time)
# ------------------------------------------------------------------------------
if (ENABLE_LOGGING)
    target_compile_definitions(csurfer PRIVATE
        ENABLE_LOGGING=1
        LOG_LEVEL_${LOG_LEVEL}=1
    )
    message(STATUS "Logging enabled (level: ${LOG_LEVEL})")
else()
    target_compile_definitions(csurfer PRIVATE ENABLE_LOGGING=0)
    message(STATUS "Logging disabled")
endif()

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
if (GTest_ADDED)
    enable_testing()
    
    # Config Tests
    add_executable(test_config
        unit-tests/config/test_config.cpp
    )
    target_link_libraries(test_config PRIVATE GTest::gtest_main csurfer_lib)
    add_test(NAME ConfigTests COMMAND test_config)

    # URL Tests
    add_executable(test_url
        unit-tests/url/test_url.cpp
    )
    target_link_libraries(test_url PRIVATE GTest::gtest_main csurfer_lib)
    add_test(NAME UrlTests COMMAND test_url)

    # String Utils Tests
    add_executable(test_string_utils
        unit-tests/utils/test_string_utils.cpp
    )
    target_link_libraries(test_string_utils PRIVATE GTest::gtest_main csurfer_lib)
    add_test(NAME StringUtilsTests COMMAND test_string_utils)

    # Parser Tests
    add_executable(test_parser
        unit-tests/utils/test_parser.cpp
    )
    target_link_libraries(test_parser PRIVATE GTest::gtest_main csurfer_lib)
    add_test(NAME ParserTests COMMAND test_parser)

    # Geometry Tests
    add_executable(test_geometry
        unit-tests/utils/test_geometry.cpp
    )
    target_link_libraries(test_geometry PRIVATE GTest::gtest_main csurfer_lib)
    add_test(NAME GeometryTests COMMAND test_geometry)
endif()

add_executable(test_color unit-tests/gfx/test_color.cpp)
target_link_libraries(test_color PRIVATE csurfer_lib GTest::gtest_main)
add_test(NAME ColorTests COMMAND test_color)

add_executable(test_http_request unit-tests/request/test_http_request.cpp)
target_link_libraries(test_http_request PRIVATE csurfer_lib GTest::gtest_main)
add_test(NAME HttpRequestTests COMMAND test_http_request)
