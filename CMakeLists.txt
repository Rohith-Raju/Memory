cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mymalloc)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_library(mymalloc_lib STATIC
  src/linear_allocator.cpp
  src/pool_allocator.cpp
)
target_include_directories(mymalloc_lib PUBLIC include)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE mymalloc_lib)

add_executable(mymalloc_tests
  tests/test_pool.cpp
  tests/test_linear.cpp
)
target_link_libraries(mymalloc_tests PRIVATE mymalloc_lib GTest::gtest_main)

enable_testing()
add_test(NAME mymalloc_tests COMMAND mymalloc_tests)


