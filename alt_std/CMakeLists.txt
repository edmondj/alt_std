cmake_minimum_required (VERSION 3.14)

add_library(alt_std INTERFACE
	"functional.h"
)

target_include_directories(alt_std
	INTERFACE ".."
)

target_compile_features(alt_std INTERFACE cxx_std_17)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET alt_std PROPERTY CXX_STANDARD 20)
endif()
