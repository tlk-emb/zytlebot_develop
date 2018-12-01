cmake_minimum_required(VERSION 2.8.3)
project(trafficlight)

find_package(catkin REQUIRED COMPONENTS
  sensor_msgs
  move_base_msgs
  cv_bridge
  roscpp
  std_msgs
  image_transport
  geometry_msgs
)

find_package(OpenCV REQUIRED)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS} 
)


add_executable(trafficlight src/trafficlight.cpp)
target_link_libraries(trafficlight ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})