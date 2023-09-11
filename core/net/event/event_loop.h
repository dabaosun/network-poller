/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-17 21:23:18
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-29 10:57:49
 * @FilePath: /network-poller/core/net/event/event_loop.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#pragma once

#include "base/noncopyable.h"
#include <functional>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <readerwriterqueue.h>

#include "net/types/net_types.h"
#include "net/poller/poller.h"

namespace liang
{
    namespace net
    {
        class Poller;
        class EventChannel;

        class EventLoop : base::noncopyable
        {
        public:
            EventLoop(const std::string &name);
            ~EventLoop();
            void Loop();
            void Quit();

            std::string Name() const
            {
                return name_;
            }

            std::thread::id OwnerThreadId() const
            {
                return thread_id_;
            }

            bool IsInLoopThread()
            {
                return std::this_thread::get_id() == thread_id_;
            }

        public:
            void RunInLoop(Functor callback);
            void QueueInLoop(Functor callback);

            void UpdateEventChannel(EventChannel *evt_channel);
            void RemoveEventChannel(EventChannel *evt_channel);

            void Wakeup(); // wake up the channel itself and handle the event on it. eg: quit.
            std::thread::id OwnerThreadId()
            {
                return thread_id_;
            }

        private:
            void HandleEventContext(const EventConextList &evt_channel_list);
            void RunTaskFuncs();

            void HandleWakeupEvent();

        private:
            std::thread::id thread_id_;
            std::string name_;
            std::atomic_bool quit_;
            bool looping_;
            std::unique_ptr<Poller> poller_; // each poller is only hold by loop thread.

            int wakeup_fd_;
            EventChannel *wakeup_channel_;

            EventConextList evt_channel_list_; // the io event channel list which handled by the loop.
            EventChannel *evt_channel_active_; // the active event channel which the loop is handling.

            // std::mutex queue_tasks_mutex_;
            // std::vector<Functor> queue_task_funcs_; // the task list which wait for loop
            moodycamel::ReaderWriterQueue<Functor> queue_task_funcs_;
        };
    }
}