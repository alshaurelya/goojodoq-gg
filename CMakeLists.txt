cmake_minimum_required(VERSION 3.10)
project(WearableHeartbeatMonitor)

# Use modern CMake practices
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories per target
include_directories(include)

# Find JsonCpp (must be available via vcpkg or system)
find_package(jsoncpp CONFIG REQUIRED)

# Server target
add_executable(server
    src/server.cpp
    src/file_utils.cpp
    src/analysis.cpp
)
target_include_directories(server PRIVATE include)
target_link_libraries(server PRIVATE jsoncpp_lib ws2_32)

# Client target
add_executable(client
    src/client.cpp
)
target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE ws2_32)
