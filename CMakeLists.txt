cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

include(CMakeDependentOption)

project(LunarYueTools VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ASSET_DIR "data")
add_subdirectory(source)
