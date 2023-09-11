/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-17 11:22:11
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-09-11 12:35:14
 * @FilePath: /network-poller/core/thread/tests/thread_test.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>
#include <thread>
#include "thread/thread.h"
#include "base/log/logger.h"

void create_func(const char *name, unsigned int length)
{
    liang::thread::EncThread thread_(name);
    thread_.Start();
}

#define THREAD_NUM 10

int main()
{
    liang::thread::EncThread *threadPool[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++)
    {
        char name[32];
        snprintf(name, sizeof(name), "thread_%d", i);
        threadPool[i] = new liang::thread::EncThread(std::string(name, sizeof(name)));
        LOG_LOG("new thread");

        threadPool[i]->Start();
        LOG_LOG("thread run, name: {0} , tId: {1} \n", threadPool[i]->GetName(),
               threadPool[i]->GetThreadID());
    }

    while (1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}