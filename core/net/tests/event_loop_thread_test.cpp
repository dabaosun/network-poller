/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-20 19:53:40
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-08-21 16:30:30
 * @FilePath: /network-poller/core/net/tests/event_loop_thread_test.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <unistd.h>
#include "net/event/event_loop_thread.h"
#include "net/event/event_loop.h"
#include "base/log/logger.h"

using namespace liang::net;

void print(EventLoop *p = NULL)
{
    LOG_LOG("print: pid = {0}, tid = {1}\n",
            getpid(), std::this_thread::get_id());
}

void quit(EventLoop *p)
{
    print(p);
    p->Quit();
}

int main()
{
    LOG_LOG("main, pid: {0}, td: {1}",getpid(),std::this_thread::get_id());
    {
        EventLoopThread threadNever("test_eventloop_thread_never_run");
    }
    {
        EventLoopThread threadDtor("test_eventloop_thread_print");
        threadDtor.Start();
        EventLoop *loopPrint = threadDtor.GetLoopInThread();
        loopPrint->RunInLoop(std::bind(print, loopPrint));

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    {
        EventLoopThread threadQuit("test_eventloop_thread_quit");
        threadQuit.Start();
        EventLoop *loopQuit = threadQuit.GetLoopInThread();
        loopQuit->RunInLoop(std::bind(quit, loopQuit));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    int i=0;
}