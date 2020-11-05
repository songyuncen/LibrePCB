# CMake version configuration
cmake_minimum_required(VERSION 3.2...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Define project
project(librepcb
    VERSION 0.1.4
    DESCRIPTION "A powerful, innovative and intuitive EDA tool for everyone"
    HOMEPAGE_URL "https://librepcb.org/"
    LANGUAGES CXX)

# Global options
option(UNBUNDLE_CMARK "Don't use vendored cmark library." OFF)
option(UNBUNDLE_ALL "Don't use any vendored library." OFF)

# Auto-include current directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Find required Qt packages
find_package(Qt5 5.2.0 COMPONENTS Widgets OpenGL Network Sql Svg PrintSupport REQUIRED)

# Find third party libraries
find_package(TypeSafe REQUIRED)
find_package(DelaunayTriangulation REQUIRED)
find_package(Optional REQUIRED)
find_package(Sexpresso REQUIRED)
find_package(QuaZip REQUIRED)

# Add libs
add_subdirectory(libs/librepcb/common)
add_subdirectory(libs/librepcb/library)
add_subdirectory(libs/librepcb/project)
add_subdirectory(libs/librepcb/workspace)
add_subdirectory(libs/librepcb/projecteditor)
add_subdirectory(libs/librepcb/libraryeditor)
add_subdirectory(libs/librepcb/librarymanager)

# Add apps
#add_subdirectory(apps/librepcb)
