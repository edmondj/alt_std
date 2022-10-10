cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(alt_std_tests
	"functional.cpp"
)

target_link_libraries(alt_std_tests
	PUBLIC alt_std GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(alt_std_tests)