cmake_minimum_required(VERSION 3.8.2)
project(RDT_Protocol)

set(CMAKE_CXX_STANDARD 14)

set(SERVER_SOURCE_FILES server/server.cpp server/server.h server/parser/server_parser.cpp server/parser/server_parser.h)
set(CLIENT_SOURCE_FILES client/parser/client_parser.cpp client/parser/client_parser.h client/client.cpp client/client.h shared/datagram_util.h)
set(SHARED_SOURCE_FILES shared/datagram.h shared/datagram.cpp server/filereader/datagram_builder.h server/filereader/datagram_builder.cpp)

add_executable(server ${SHARED_SOURCE_FILES} ${SERVER_SOURCE_FILES})
add_executable(client ${SHARED_SOURCE_FILES} ${CLIENT_SOURCE_FILES})
add_executable(dummy_client dummy_client.cpp)
