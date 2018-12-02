cmake_minimum_required(VERSION 2.8.3)
project(trafficlight)

set(CMAKE_CXX_FLAGS "g++ -std=c++11 -lm -g -fsigned-char -fpermissive -Wint-to-pointer-cast -O3")

find_package(catkin REQUIRED COMPONENTS
  sensor_msgs
  move_base_msgs
  cv_bridge
  roscpp
  std_msgs
  image_transport
  geometry_msgs
)

catkin_package(
)

find_package(OpenCV REQUIRED)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS} 
)


add_executable(benchmark src/ros_benchmark.cpp src/xcalc_hog.c src/xcalc_hog_linux.c  src/xcalc_hog_sinit.c  src/xcolor_hist.c src/xcolor_hist_linux.c src/xcolor_hist_sinit.c src/linuxmisc.cpp src/linuxmmap.cpp)
add_executable(realtime src/realtime.cpp src/xcalc_hog.c src/xcalc_hog_linux.c  src/xcalc_hog_sinit.c  src/xcolor_hist.c src/xcolor_hist_linux.c src/xcolor_hist_sinit.c src/linuxmisc.cpp src/linuxmmap.cpp)
target_link_libraries(benchmark ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(realtime ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})