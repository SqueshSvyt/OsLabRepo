cmake_minimum_required(VERSION 3.0)
project(OS)

set(CMAKE_CXX_STANDARD 17)

set(App main.cpp)

if(WIN32)
    list(APPEND App FotWin32.cpp)
elseif(UNIX)
    list(APPEND App ForUnix.cpp)
endif()

add_executable(OS ${App})