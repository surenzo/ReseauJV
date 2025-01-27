cmake_minimum_required(VERSION 3.30)
project(ReseauJV)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

if(WIN32)
    set(PLATFORM_SOURCES src/Socket_windows.cpp)
elseif(UNIX)
    set(PLATFORM_SOURCES src/Socket_posix.cpp)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_library(Socket STATIC
    ${PLATFORM_SOURCES}  
    src/Socket_common.cpp
)

add_executable(Server src/server.cpp)
target_link_libraries(Server PRIVATE Socket)

add_executable(Client src/client.cpp)
target_link_libraries(Client PRIVATE Socket)
