cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 17)

project(mymalloc)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE 
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE 
	src/linear_allocator.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include)


