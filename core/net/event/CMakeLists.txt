set(event_SRCS
    net/event/event_channel.cpp
    net/event/event_loop_thread_pool.cpp
    net/event/event_loop_thread.cpp
    net/event/event_loop.cpp
    net/event/event.cpp
    PARENT_SCOPE)

message(STATUS "event source files:" ${event_SRCS})
