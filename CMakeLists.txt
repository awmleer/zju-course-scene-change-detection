cmake_minimum_required(VERSION 3.9)
project(scene_change_detection)

set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(scene_change_detection main.cpp)

target_link_libraries (scene_change_detection ${OpenCV_LIBS})