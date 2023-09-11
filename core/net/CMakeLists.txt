add_subdirectory(event)
add_subdirectory(poller)
add_subdirectory(poller_impl)
add_subdirectory(socket)
add_subdirectory(tcp)

set(net_SRCS
  ${event_SRCS}
  ${poller_impl_SRCS}
  ${poller_SRCS}
  ${socket_SRCS}
  ${tcp_SRCS}
  PARENT_SCOPE
)