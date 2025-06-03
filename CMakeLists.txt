cmake_minimum_required(VERSION 3.10)
project(WearableHeartbeatMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

# Server target
add_executable(server
    src/server.cpp
    src/file_utils.cpp
    src/analysis.cpp
)
target_include_directories(server PRIVATE include)
target_link_libraries(server PRIVATE nlohmann_json_bad_name ws2_32) 


add_executable(client
    src/client.cpp
)
target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE ws2_32)