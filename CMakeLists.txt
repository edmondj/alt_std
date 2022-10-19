cmake_minimum_required (VERSION 3.14)

project ("alt_std")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_modules)

option(ENABLE_RT "Enable C++ runtime when building" ON)

if (MSVC)
	add_compile_options("/W3" "/WX")
	if (NOT ENABLE_RT)
		add_compile_options("/GR-")
	endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
	add_compile_options("-Wextra" "-Werror")
	if (NOT ENABLE_RT)
		add_compile_options("-fno-rtti" "-fno-exceptions")
	endif ()
endif ()

add_subdirectory("alt_std")
add_subdirectory("tests")