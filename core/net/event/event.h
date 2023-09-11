/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-19 21:16:52
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-22 16:38:08
 * @FilePath: /network-poller/core/net/event/event.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#pragma once

#include "base/noncopyable.h"

namespace liang
{
    namespace net
    {
        class EventLoop;

        class Event : base::noncopyable
        {
        public:
            static const int none_event_flag;
            static const int read_event_flag;
            static const int write_event_flag;
            static const int error_event_flag;

        public:
            Event(EventLoop *event_loop, int fd);

        private:
            int event_fd_;            // event' fd
            EventLoop *event_loop_; // loop which handles the event
        };
    }
}
