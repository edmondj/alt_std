cmake_minimum_required (VERSION 3.14)

include(FetchContent)
FetchContent_Declare(benchmark
	GIT_REPOSITORY https://github.com/google/benchmark
	GIT_TAG v1.7.0
	INSTALL_DIR ${CMAKE_BINARY_DIR}/deps/benchmark
	CMAKE_ARGS -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DINSTALL_DIR=${CMAKE_BINARY_DIR}/deps/benchmark
	PATCH_COMMAND git checkout . && git apply ${CMAKE_CURRENT_SOURCE_DIR}/benchmark.patch
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

add_executable(alt_std_benchmark
	"functional.cpp"
)

target_link_libraries(alt_std_benchmark
	PUBLIC alt_std benchmark::benchmark_main benchmark::benchmark
)
