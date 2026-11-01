cmake_minimum_required(VERSION 3.20)
project(UnifiedEngineeringWorkbench 
    VERSION 1.0.0 
    DESCRIPTION "PASS Science Works — Unified Engineering Workbench"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable position independent code and exports
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Configure output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler warnings and optimizations
if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8 /fp:precise)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find Qt 6 Packages
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Concurrent)
qt_standard_project_setup()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# ---------------------------------------------------------------------------
# 1. Core Numerical & Engineering Library (Pure C++17, Zero Qt dependency)
# ---------------------------------------------------------------------------
file(GLOB_RECURSE CORE_SOURCES 
    src/core/*.cpp
    src/core/*.hpp
    src/core/*.h
)

add_library(WorkbenchCore STATIC ${CORE_SOURCES})
target_include_directories(WorkbenchCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)
target_compile_features(WorkbenchCore PUBLIC cxx_std_17)

# ---------------------------------------------------------------------------
# 2. Main Desktop Workbench Application (Qt 6 Widgets + OpenGL)
# ---------------------------------------------------------------------------
file(GLOB_RECURSE APP_SOURCES 
    src/app/*.cpp
    src/app/*.hpp
    src/app/*.h
)

qt_add_executable(WorkbenchApp WIN32 ${APP_SOURCES})
target_link_libraries(WorkbenchApp PRIVATE 
    WorkbenchCore
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Concurrent
)

# ---------------------------------------------------------------------------
# 3. Standalone Launch.exe Wrapper (Zero-dependency native Win32 starter)
# ---------------------------------------------------------------------------
add_executable(Launch WIN32 packaging/Launch.cpp)
target_compile_features(Launch PRIVATE cxx_std_17)

# ---------------------------------------------------------------------------
# 4. Automated Test Suite (CTest)
# ---------------------------------------------------------------------------
enable_testing()

file(GLOB_RECURSE TEST_FILES tests/*.cpp)
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE WorkbenchCore)
    target_include_directories(${TEST_NAME} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
