cmake_minimum_required(VERSION 3.0.12)

set(CMAKE_CXX_COMPILER "g++")

project(web_server)

set(EXEC_NAME server)

add_executable(${EXEC_NAME} main.cpp
                            network/SocketHolder.cpp
                            network/Server.cpp
                            parsing/CfgCtx.cpp
                            body_handler/UploadBodyHandler.cpp)

target_compile_options(server PRIVATE "-fsanitize=address")
target_link_options(server PRIVATE "-fsanitize=address")

target_compile_options(server PUBLIC "-std=c++98")
