/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-17 20:41:31
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-08-20 08:48:22
 * @FilePath: /network-poller/core/net/poller/poller.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "poller.h"
#include "net/poller_impl/epoll_poller.h"
#include "net/event/event_loop.h"

namespace liang
{
    namespace net
    {
        Poller *Poller::NewPoller(EventLoop *loop)
        {
            return new EpollPoller(loop);
        }

        Poller::Poller(EventLoop *loop) : loop_(loop),
                                          thread_id_(loop_->OwnerThreadId())
        {
            assert(std::this_thread::get_id() == loop_->OwnerThreadId());
        }

        Poller::~Poller()
        {
        }

        void Poller::Poll(int timeoutMS, EventConextList *active_ctx_list)
        {
            assert(IsOwnerThread(*this));
        }

        void Poller::UpdateEventChannel(EventChannel *ctx)
        {
            assert(IsOwnerThread(*this));
        }

        void Poller::RemoveEventChannel(EventChannel *ctx)
        {
            assert(IsOwnerThread(*this));
        }
    }
}
