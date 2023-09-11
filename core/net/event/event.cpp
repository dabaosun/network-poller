/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-17 21:20:17
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-29 08:43:44
 * @FilePath: /network-poller/core/net/event/event.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "event.h"

#include <sys/epoll.h>
#include "net/event/event_loop.h"

namespace liang
{
    namespace net
    {
        const int Event::none_event_flag = 0;
        const int Event::read_event_flag = EPOLLIN | EPOLLPRI |EPOLLET;
        const int Event::write_event_flag = EPOLLOUT;
        const int Event::error_event_flag = EPOLLERR;

        Event::Event(EventLoop *event_loop, int fd)
        {
        }
    }
}
