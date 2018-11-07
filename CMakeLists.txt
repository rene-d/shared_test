
cmake_minimum_required(VERSION 3.5)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(CMAKE_CXX_COMPILER g++-8)
set(CMAKE_C_COMPILER gcc-8)
endif()

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system chrono)

add_executable(shared shared.cpp)
target_link_libraries(shared PRIVATE boost_chrono boost_system)
