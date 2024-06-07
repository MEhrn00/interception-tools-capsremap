cmake_minimum_required(VERSION 3.15)

project(
  capsremap
  VERSION 0.0.1
  DESCRIPTION "interception tools plugin for remapping capslock"
  HOMEPAGE_URL "https://github.com/MEhrn00/interception-tools-capsremap"
  LANGUAGES CXX)

option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_IWYU "Enable include-what-you-use" OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.74 REQUIRED COMPONENTS program_options)
message(STATUS "Boost version: ${Boost_VERSION}")

include(StaticAnalyzers)
include(CompilerWarnings)

add_subdirectory(capsremap)
