/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-19 12:00:02
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-08-20 08:57:29
 * @FilePath: /network-poller/core/net/poller_impl/epoll_poller.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <sys/epoll.h>
#include "base/noncopyable.h"
#include "net/poller/poller.h"

namespace liang
{
    namespace net
    {
        class EpollPoller : public Poller
        {
        public:
            EpollPoller(EventLoop *loop);

            ~EpollPoller();

            void Poll(int timeoutMS, EventConextList *active_evt_channel) override;

            void UpdateEventChannel(EventChannel *evt_channel) override;

            void RemoveEventChannel(EventChannel *evt_channel) override;

        private:
            void UpdateEpollCtrl(int operation, EventChannel *evt_channel);

        private:
            int epoll_fd_;

            typedef std::vector<struct epoll_event> EventList;
            EventList events_ready;
        };
    }
}
