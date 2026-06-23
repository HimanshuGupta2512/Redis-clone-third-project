cmake_minimum_required(VERSION 3.10)
project(CacheLite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure WinSock2 library is linked on Windows
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# Main server executable
add_executable(cachelite 
    main.cpp
    server.cpp
    logger.cpp
    client_handler.cpp
    command_parser.cpp
    kv_engine.cpp
)

# Link Winsock2 privately
target_link_libraries(cachelite PRIVATE ws2_32)
