/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-17 19:58:49
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-08-20 08:57:18
 * @FilePath: /network-poller/core/net/poller/poller.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#pragma once

#include "base/noncopyable.h"
#include <vector>
#include <assert.h>
#include <unordered_map>
#include "net/types/net_types.h"

using namespace liang::base;
namespace liang
{
    namespace net
    {
        class EventLoop;
        class EventChannel;

        class Poller : noncopyable
        {

        public:
            Poller(EventLoop *loop);
            virtual ~Poller();

            virtual void Poll(int timeoutMS, EventConextList *active_ctx_list);

            virtual void UpdateEventChannel(EventChannel *evt_channel); // add/update event channel into poller and set ctrl

            virtual void RemoveEventChannel(EventChannel *evt_channel); // remove event channel from poller

            static Poller *NewPoller(EventLoop *loop);

            std::thread::id OwnerThreadId() const
            {
                return thread_id_;
            }

        protected:
            EventLoop *loop_;
            std::thread::id thread_id_;
            std::unordered_map<int, EventChannel *> evt_channel_list_;

            void AssertInLoopThread();
        };

    } // namespace net
} // namespace liang