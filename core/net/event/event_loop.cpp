/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-18 09:50:05
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-29 11:08:06
 * @FilePath: /network-poller/core/net/event/event_loop.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "event_loop.h"
#include <memory>
#include <assert.h>
#include <algorithm>
#include <thread>
#include <sys/eventfd.h>
#include <sstream>
#include <poll.h>

#include "net/event/event_channel.h"
#include "thread/thread.h"
#include "net/poller/poller.h"
#include "base/log/logger.h"

namespace liang
{
    namespace net
    {
        thread_local static EventLoop *evt_loop_this_thread = nullptr;

        EventLoop::EventLoop(const std::string &name) : name_(name),
                                                        thread_id_(std::this_thread::get_id()),
                                                        quit_(false),
                                                        looping_(false),
                                                        poller_(Poller::NewPoller(this)),
                                                        wakeup_fd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
                                                        wakeup_channel_(new EventChannel(this, wakeup_fd_)),
                                                        queue_task_funcs_(1024*65536)
        {
            assert(evt_loop_this_thread == nullptr);
            evt_loop_this_thread = this;
            LOG_DEBUG("event loop created, name: {0}, thread: {1}", name_, thread_id_);
            wakeup_channel_->SetReadEventCallBack(std::bind(&EventLoop::HandleWakeupEvent, this));
            wakeup_channel_->EnableReading();
        }

        EventLoop::~EventLoop()
        {
            LOG_DEBUG("event loop destory, name: {0}, thread: {1}", name_, thread_id_);

            wakeup_channel_->DisableAll();     // delete from poller
            wakeup_channel_->RemoveFromLoop(); // delete from loop
            ::close(wakeup_fd_);
            wakeup_fd_ = 0;
            evt_loop_this_thread = nullptr;
        }

        void EventLoop::Loop()
        {
            looping_ = true;
            quit_ = false;
            LOG_LOG("EventLoop start loop, name: {0}", name_);
            while (!quit_)
            {
                evt_channel_list_.clear();
                // 1. handle the IO event context
                // 1.1 poll the IO event to context to evt_channel_list_
                poller_->Poll(0, &evt_channel_list_);
                // LOG_TRACE("{0} polled actived event channel count: {1}", name_, evt_channel_list_.size());
                //   1.2 start to handle the event of every context.
                HandleEventContext(evt_channel_list_);
                // 2. handle the pending task funcs
                RunTaskFuncs();
            }

            looping_ = false;
        }

        void EventLoop::Quit()
        {
            quit_ = true;
            if (!IsOwnerThread(*this))
            {
                Wakeup();
            }
        }

        void EventLoop::RunInLoop(Functor callback)
        {
            if (IsOwnerThread(*this))
            {
                // current thread is loop thread, so directly run the task func;
                callback();
            }
            else
            {
                // add task func to queue to wait for loop thread.
                QueueInLoop(callback);
            }
        }

        void EventLoop::QueueInLoop(Functor callback)
        {
            // std::unique_lock<std::mutex> lock(queue_tasks_mutex_);
            // queue_task_funcs_.push_back(callback);
            queue_task_funcs_.enqueue(callback);
        }

        void EventLoop::UpdateEventChannel(EventChannel *evt_channel)
        {
            poller_->UpdateEventChannel(evt_channel);
        }

        void EventLoop::RemoveEventChannel(EventChannel *evt_channel)
        {
            assert(evt_channel->OwnerLoop() == this);
            assert(std::this_thread::get_id() == this->thread_id_);
            if (nullptr != evt_channel_active_)
            {
                assert(evt_channel == evt_channel_active_ || std::find(evt_channel_list_.begin(), evt_channel_list_.end(), evt_channel) == evt_channel_list_.end());
            }
            poller_->RemoveEventChannel(evt_channel);
        }

        void EventLoop::Wakeup()
        {
            uint32_t one;
            int n = ::write(wakeup_fd_, &one, sizeof(one));
            if (sizeof(one) != n)
            {
                LOG_ERROR("writes {0} bytes, not {1} expected bytes", n, sizeof(one));
            }
        }

        void EventLoop::HandleEventContext(const EventConextList &ctx_list)
        {
            evt_channel_active_ = NULL;
            for (auto ctx : ctx_list)
            {
                evt_channel_active_ = ctx;
                evt_channel_active_->HandleEvent();
            }
            evt_channel_active_ = NULL;
        }

        void EventLoop::RunTaskFuncs()
        {
            // std::vector<Functor> task_func_list;
            // {
            //     std::unique_lock<std::mutex> lock(queue_tasks_mutex_);
            //     task_func_list.swap(queue_task_funcs_);
            // }
            // for (auto task_func : task_func_list)
            // {
            //     task_func();
            // }

            Functor task;
            //while (true)
            {
                if (queue_task_funcs_.try_dequeue(task))
                {
                    // Process task...
                    task();
                }
                else
                {
                    //break;
                }
            }
        }

        void EventLoop::HandleWakeupEvent()
        {
            uint32_t one;
            int n = ::read(wakeup_fd_, &one, sizeof(one));
            if (sizeof(one) != n)
            {
                LOG_ERROR("reads {0} bytes, not {1} expected bytes", n, sizeof(one));
            }
        }
    }
}
