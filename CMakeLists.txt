cmake_minimum_required(VERSION 3.29)
project(PortManager_backend C)

set(CMAKE_C_STANDARD 11)

add_executable(PortManager_backend main.c
        DatabaseUtil.h)

target_link_libraries(PortManager_backend mariadb)
