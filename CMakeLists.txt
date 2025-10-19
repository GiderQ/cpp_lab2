cmake_minimum_required(VERSION 3.30)
project(cpp_lab2)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)

    set(CMAKE_CXX_FLAGS_DEBUG "/Od" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2" CACHE STRING "" FORCE)

endif()

add_executable(cpp_lab2 main.cpp)