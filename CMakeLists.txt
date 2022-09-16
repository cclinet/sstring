cmake_minimum_required(VERSION 3.23)
project(sstring)

set(CMAKE_CXX_STANDARD 20)
include(FetchContent)
#
#FetchContent_Declare(GSL
#        GIT_REPOSITORY "https://github.com/microsoft/GSL"
#        GIT_TAG "v4.0.0"
#        )
#FetchContent_MakeAvailable(GSL)


add_executable(sstring main.cpp)
target_include_directories(sstring PRIVATE include)
#target_link_libraries(sstring PRIVATE GSL)