
message(status ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(thread)

set(core_SRCS
  ${base_SRCS}
  ${net_SRCS}
  ${thread_SRCS}
)

message(STATUS "core libray's codes: " ${core_SRCS})

# add library
add_library(liang_core SHARED ${core_SRCS})

# link library
target_link_libraries(liang_core pthread rt)

# install lib to path
install(TARGETS liang_core DESTINATION lib)

message(STATUS "INCLUDE_DIRECTORIES - ${INCLUDE_DIRECTORIES}")
message(STATUS "CXX_FLAGS: ${CXX_FLAGS}")

if(LIANG_BUILD_TESTS)
  add_subdirectory(thread/tests)
  add_subdirectory(net/tests)

endif()
