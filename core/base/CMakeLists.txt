# file(GLOB HEADERS "*.h")
# install(FILES ${HEADERS} DESTINATION include/core/base)
# message(STATUS "base header files : ${HEADERS}")

set(base_SRCS
    base/buffer/buffer.cpp
    base/log/logger.cpp
    PARENT_SCOPE
)