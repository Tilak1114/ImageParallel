cmake_minimum_required(VERSION 3.24)
project(ImageParallel)

set(CMAKE_CXX_STANDARD 17)

add_executable(ImageParallel main.cpp)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( ImageParallel ${OpenCV_LIBS})
