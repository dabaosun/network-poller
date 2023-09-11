/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-19 11:49:14
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-23 20:58:38
 * @FilePath: /network-poller/core/net/types/net_types.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <ostream>

namespace liang
{
    namespace net
    {
        class EventChannel;

        typedef std::function<void()> Functor;
        typedef std::vector<EventChannel *> EventConextList;

        template <class T>
        bool IsOwnerThread(const T &obj)
        {
            return (std::this_thread::get_id() == obj.OwnerThreadId());
        }

    }
}