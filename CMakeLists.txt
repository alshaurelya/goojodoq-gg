cmake_minimum_required(VERSION 3.10)
project(WearableHeartbeatMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Server target
add_executable(server
    src/server.cpp
    src/file_utils.cpp
    src/analysis.cpp
)
target_include_directories(server PRIVATE include) # nlohmann::json akan ditemukan dari sini
# Hanya link ws2_32, nlohmann::json adalah header-only
target_link_libraries(server PRIVATE ws2_32)

# Client target
add_executable(client
    src/client.cpp
)
target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE ws2_32)