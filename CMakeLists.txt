cmake_minimum_required(VERSION 3.8.2)
project(RDT_Protocol)

set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_STANDARD 14)

set(SERVER_SOURCE_FILES
        server/server.cpp server/server.h
        server/parser/server_parser.cpp
        server/parser/server_parser.h
        server/worker_thread.cpp
        server/worker_thread.h
        server/rdt_strategy.h
        server/selective_repeat_strategy.h
        server/selective_repeat_strategy.cpp)

set(CLIENT_SOURCE_FILES
        client/parser/client_parser.cpp
        client/parser/client_parser.h
        client/client.cpp client/client.h)

set(SHARED_SOURCE_FILES
        shared/data_packet.h
        shared/data_packet.cpp
        shared/ack_packet.cpp
        shared/ack_packet.h
        shared/packet_util.h
        server/filereader/packet_builder.h
        server/filereader/packet_builder.cpp)

add_executable(server ${SHARED_SOURCE_FILES} ${SERVER_SOURCE_FILES})
add_executable(client ${SHARED_SOURCE_FILES} ${CLIENT_SOURCE_FILES})
add_executable(dummy_client dummy_client.cpp)