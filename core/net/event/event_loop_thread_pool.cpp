/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-21 16:49:04
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-25 14:03:36
 * @FilePath: /network-poller/core/net/event/event_loop_thread_pool.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "event_loop_thread_pool.h"
#include "net/event/event_loop_thread.h"
#include "net/event/event_loop.h"

namespace liang
{
    namespace net
    {
        EventLoopThreadPool::EventLoopThreadPool(const char *name, const uint32_t num, EventLoop *loop) : pool_name_(name),
                                                                                                          thread_num_(num),
                                                                                                          main_loop_(loop),
                                                                                                          owner_thread_id_(std::this_thread::get_id())
        {
        }

        EventLoopThreadPool::~EventLoopThreadPool()
        {
        }

        void EventLoopThreadPool::Initialize()
        {
            for (int i = 0; i < thread_num_; i++)
            {
                char name[64]{0};
                snprintf(name, sizeof(name), "%s_loop_thread_%d", pool_name_.c_str(),i);
                EventLoopThread *loopThread = new EventLoopThread(std::string(name, sizeof(name)));
                loopThread->Start();
                thread_list_.push_back(std::unique_ptr<EventLoopThread>(loopThread));
                loop_list_.push_back(loopThread->GetLoopInThread());
            }
        }

        EventLoop *EventLoopThreadPool::GetEventLoopFromPool()
        {
            EventLoop *loop = main_loop_;
            if (!loop_list_.empty())
            {
                // round-robin
                loop = loop_list_[next_index_];
                ++next_index_;
                if (next_index_ >= static_cast<int>(loop_list_.size()))
                {
                    next_index_ = 0;
                }
            }
            return loop;
        }

        EventLoop *EventLoopThreadPool::GetEventLoopFromPoolForHash(const uint32_t &hash_code)
        {
            if (loop_list_.size() == 0)
                return main_loop_;
            uint32_t hash_index = hash_code % loop_list_.size();
            return loop_list_[hash_index];
        }
    } // namespace net
}
