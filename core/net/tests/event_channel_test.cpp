/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-20 09:20:02
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-09-11 10:56:20
 * @FilePath: /network-poller/core/net/tests/event_channel_test.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <unistd.h>
#include <thread>
#include <sstream>

#include "net/event/event_loop.h"
#include "base/log/logger.h"

using namespace liang::net;

void testFunc()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    LOG_DEBUG("testFunc(), pid: {0}, tid: {1}\n", getpid(), ss.str());
    EventLoop loop("test_func_loop");
    loop.Loop();
}

int main(int argv, char **args)
{
    SET_LOG_LEVEL(LEVEL_TRACE);
    std::stringstream ss;
    ss << std::this_thread::get_id();
    LOG_DEBUG("main(), pid: {0}, tid: {1}", getpid(), ss.str());

    std::thread testThread(testFunc);

    EventLoop loop("main_test_loop");
    loop.Loop();
}