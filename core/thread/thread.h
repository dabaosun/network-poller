/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-16 17:17:30
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-09-11 12:35:38
 * @FilePath: /network-poller/core/thread/thread.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <thread>
#include <memory>
#include <mutex>
#include <functional>
#include <condition_variable>
#include "base/noncopyable.h"

namespace liang
{
    namespace thread
    {
        class EncThread
        {
        public:
            explicit EncThread(const std::string &name);
            virtual ~EncThread();

        public:
            void Start(); // start the thread.
            void Join();  // join the thread.
            std::thread::id GetThreadID();
            static std::thread::id GetCurrentID();

            std::string GetName()
            {
                return name_;
            };

        protected:
            bool quited_ = false;
            virtual void run();

        protected:
            std::string name_;
            bool started_ = false;
            bool joined_ = false;
            std::shared_ptr<std::thread> threadobj_;

            std::mutex mutex_;
            std::condition_variable condition_;

            unsigned long int thread_t_; // thread identifiers
            int *tid_;                   // thread id

        private:
            void ThreadFunc();
        };

    } // namespace thread
} // namespace liang
