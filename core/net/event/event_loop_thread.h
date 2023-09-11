/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-20 19:05:56
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-08-20 21:57:41
 * @FilePath: /network-poller/core/net/event/event_loop_thread.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#pragma once

#include <string>
#include <functional>
#include "base/noncopyable.h"
#include "thread/thread.h"

using namespace liang::thread;

namespace liang
{
    namespace net
    {
        class EventLoop;

        class EventLoopThread : public EncThread
        {
        public:
            typedef std::function<void(EventLoop *)> ThreadInitCallback;

            EventLoopThread(const std::string &name);
            ~EventLoopThread();

            EventLoop *GetLoopInThread(); // return loop

        private:
            EventLoop *loop_;
            std::mutex mutex_;
            std::condition_variable cond_;
            ThreadInitCallback init_cb_;

        protected:
            virtual void run();
        };
    }
}