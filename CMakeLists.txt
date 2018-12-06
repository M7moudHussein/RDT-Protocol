cmake_minimum_required(VERSION 3.8.2)
project(RDT_Protocol)

set(CMAKE_CXX_STANDARD 14)

set(SERVER_SOURCE_FILES
        server.cpp
        server.h
        server_parser.cpp
        server_parser.h)

set(CLIENT_SOURCE_FILES
        client_parser.cpp
        client_parser.h
        client.cpp client.h)

set(SHARED_SOURCE_FILES
        datagram.h
        datagram.cpp
        datagram_builder.h
        datagram_builder.cpp
        )

add_executable(server
        ${SHARED_SOURCE_FILES}
        ${SERVER_SOURCE_FILES}
        )
add_executable(client
        ${SHARED_SOURCE_FILES}
        ${CLIENT_SOURCE_FILES}
        )