
set(poller_impl_SRCS
    net/poller_impl/epoll_poller.cpp
    PARENT_SCOPE)

message(STATUS "poller_impl source files:" ${poller_impl_SRCS})