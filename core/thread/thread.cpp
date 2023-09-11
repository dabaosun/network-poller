/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-16 17:19:43
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-09-11 12:36:59
 * @FilePath: /network-poller/core/thread/thread.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <memory>
#include "thread.h"
#include "base/log/logger.h"
        
namespace liang
{
    namespace thread
    {

        
        EncThread::EncThread(const std::string &name)
            : name_(name),
              started_(false),
              quited_(false),
              threadobj_(nullptr)
        {
            LOG_DEBUG("EncThread ctor, name: {0}", name_);
        }

        EncThread::~EncThread()
        {
            LOG_DEBUG("EncThread dctor, name: {0}", name_);
            if (nullptr == threadobj_)
                return;

            quited_ = true;
            if (threadobj_->joinable())
            {
                threadobj_->join();
            }
        }

        std::thread::id EncThread::GetCurrentID()
        {
            return std::this_thread::get_id();
        }

        void EncThread::Start()
        {
            if (nullptr != threadobj_)
                return;
            std::unique_lock<std::mutex> lock(mutex_);
            threadobj_ = std::make_shared<std::thread>(std::bind(&EncThread::ThreadFunc, this));
            pthread_setname_np(threadobj_->native_handle(), name_.c_str());
            started_ = true;
            condition_.notify_one();
        }

        void EncThread::Join()
        {
            if (threadobj_->joinable())
            {
                return threadobj_->join();
            }
        }

        std::thread::id EncThread::GetThreadID()
        {
            return threadobj_->get_id();
        }

        void EncThread::run()
        {
            while (!quited_)
            {
                LOG_TRACE("base thread running : {0} \n", GetThreadID());
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        void EncThread::ThreadFunc()
        {
            {
                std::unique_lock<std::mutex> lck(mutex_);
                if (!started_)
                {
                    condition_.wait(lck);
                    if (quited_)
                    {
                        return;
                    }
                }
            }

            run();
        }

        void runTestThread()
        {
        }
    }
}
