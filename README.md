# Introduction

network-poller is  an event-driven framework with reactor / proactor pattern.

# Objective

network-poller provides multithreaded C++ network library and allow you to write non-blocking, asynchronous code easily on both Linux and Windows and provide high performance capability.&#x20;

# Requires

*   Linux kernel version >= 2.6.28.
*   GCC >= 4.7 or Clang >= 3.5.
*   CMake >= 3.0

# Build

```bash
git clone https://github.com/dabaosun/network-poller.git
cd network-poller
mkdir build && cmake ..
```

# Feature/Roadmap

*   [x] supports Linux EPOLL sync I/O.
*   [ ] supports Linux IO\_DURING async I/O model.
*   [ ] supports Window IOCP async I/O model.
*   [ ] supports timer
*   [ ] support memory pool

# License

BSD 3-Clause License&#x20;
