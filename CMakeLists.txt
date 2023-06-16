cmake_minimum_required(VERSION 3.22)
project(01-spellchecker LANGUAGES CXX)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Werror -g")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Werror -g -O3")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "SANITIZED")
    set(CMAKE_CXX_FLAGS "-fsanitize=address,undefined,leak")
endif()

add_executable(01-spellchecker src/main.cpp)
