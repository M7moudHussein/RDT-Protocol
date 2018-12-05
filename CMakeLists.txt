cmake_minimum_required(VERSION 3.8.2)
project(RDT_Protocol)

set(CMAKE_CXX_STANDARD 11)

add_executable(RDT_Protocol main.cpp server.cpp server.h server_parser.cpp server_parser.h client_parser.cpp client_parser.h client.cpp client.h)