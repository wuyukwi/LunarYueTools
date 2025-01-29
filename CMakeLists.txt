cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(LunarYueTools VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASSET_DIR "data")
add_subdirectory(source)
