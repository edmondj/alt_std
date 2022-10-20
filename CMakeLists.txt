cmake_minimum_required (VERSION 3.14)

project ("alt_std")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_modules)

option(ENABLE_RT "Enable C++ runtime when building" ON)

function(target_alt_add_flags t)
	if (MSVC)
		target_compile_options(${t}
			INTERFACE "/WX"
		)
		if (NOT ENABLE_RT)
			target_compile_options(${t}
				INTERFACE "/GR-"
			)
		endif ()
	elseif (CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
		target_compile_options(${t}
			INTERFACE "-Wextra" "-Werror"
		)
		if (NOT ENABLE_RT)
			target_compile_options(${t}
				INTERFACE "-fno-rtti" "-fno-exceptions"
			)
		endif ()
	endif ()
endfunction()

add_subdirectory("alt_std")
add_subdirectory("tests")

option(BUILD_BENCHMARK "Build the benchmark project" OFF)

if (${BUILD_BENCHMARK})
	add_subdirectory("benchmark")
endif ()