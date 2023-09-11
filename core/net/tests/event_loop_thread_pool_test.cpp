/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-21 17:31:05
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-09-10 22:21:26
 * @FilePath: /network-poller/core/net/tests/event_loop_thread_pool_test.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <assert.h>
#include "net/event/event_loop_thread_pool.h"
#include "net/event/event_loop.h"

using namespace liang::net;

int main()
{
    EventLoop main_loop("main_loop");
    {
        const uint32_t thread_num = 10;
        EventLoopThreadPool pool("test1_threadpool",thread_num, &main_loop);
        pool.Initialize();
        for (int i = 0; i < thread_num; i++)
        {
            auto work_pool = pool.GetEventLoopFromPool();
            assert(work_pool != &main_loop);
        }
    }
    {
        const uint32_t thread_num = 0;
        EventLoopThreadPool pool("test2_threadpool", thread_num, &main_loop);
        pool.Initialize();
        for (int i = 0; i < thread_num; i++)
        {
            auto work_pool = pool.GetEventLoopFromPool();
            assert(work_pool != &main_loop);
        }
        assert(pool.GetEventLoopFromPool() == &main_loop);
    }

    main_loop.Loop();
}