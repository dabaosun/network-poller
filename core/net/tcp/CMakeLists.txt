set(tcp_SRCS
    net/tcp/tcp_acceptor.cpp
    net/tcp/tcp_client.cpp
    net/tcp/tcp_connection.cpp
    net/tcp/tcp_server.cpp
    net/tcp/tcp_connector.cpp
    PARENT_SCOPE)

message(STATUS "tcp source files:" ${tcp_SRCS})
