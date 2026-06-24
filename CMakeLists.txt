cmake_minimum_required(VERSION 3.10)
project(CacheLite CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Windows specific library
if (WIN32)
    link_libraries(ws2_32)
endif()

include_directories(include)

# Main server executable
add_executable(cachelite 
    src/main.cpp
    src/server.cpp
    src/logger.cpp
    src/client_handler.cpp
    src/command_parser.cpp
    src/kv_engine.cpp
    src/ttl_manager.cpp
    src/persistence_manager.cpp
)

# Link Winsock2 privately
target_link_libraries(cachelite PRIVATE ws2_32)

add_executable(stress_client tools/stress_client.cpp)
target_link_libraries(stress_client PRIVATE ws2_32)
target_compile_features(stress_client PRIVATE cxx_std_17)
