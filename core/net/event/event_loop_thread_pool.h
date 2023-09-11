/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-21 17:28:03
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-25 14:01:55
 * @FilePath: /network-poller/core/net/event/event_loop_thread_pool.h
 * @Description: 
 * 
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved. 
 */

#pragma once

#include <stdint.h>
#include <string.h>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

namespace liang
{
    namespace net
    {
        class EventLoop;
        class EventLoopThread;

        class EventLoopThreadPool
        {
        public:
            explicit EventLoopThreadPool(const char* name, const uint32_t num, EventLoop *loop);

            ~EventLoopThreadPool();
            
            void Initialize();

            EventLoop *GetEventLoopFromPool();
            EventLoop *GetEventLoopFromPoolForHash(const uint32_t &hash_code);

        private:
            std::string pool_name_;
            uint32_t thread_num_{0};
            EventLoop *main_loop_;
            std::thread::id owner_thread_id_;
            uint32_t next_index_{0};

            std::mutex thd_list_mutx_;
            std::vector<std::unique_ptr<EventLoopThread>> thread_list_; // pool contains the threads
            std::vector<EventLoop *> loop_list_;
        };
    }
}