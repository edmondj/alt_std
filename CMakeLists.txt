cmake_minimum_required (VERSION 3.14)

project ("alt_std")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_modules)

add_subdirectory("alt_std")
add_subdirectory("tests")