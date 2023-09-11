
set(socket_SRCS
    net/socket/socket_utility.cpp
    net/socket/socket_addr.cpp
    net/socket/socket.cpp
    PARENT_SCOPE)

message(STATUS "socket source files:" ${socket_SRCS})
