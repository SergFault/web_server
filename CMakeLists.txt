cmake_minimum_required(VERSION 3.0.12)

project(web_server)

set(EXEC_NAME server)

add_executable(${EXEC_NAME} main.cpp network/SocketHolder.cpp network/Server.cpp parsing/CfgCtx.cpp resp_writer/RespWriter.cpp)

target_compile_options(server PUBLIC "-std=c++98")