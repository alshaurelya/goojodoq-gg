cmake_minimum_required(VERSION 3.10)
project(WearableHeartbeatMonitor)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

# Automatically find JsonCpp via vcpkg
find_package(jsoncpp CONFIG REQUIRED)

add_executable(server src/server.cpp src/file_utils.cpp src/analysis.cpp)
target_link_libraries(server PRIVATE jsoncpp_lib ws2_32)

add_executable(client src/client.cpp)
target_link_libraries(client PRIVATE ws2_32)