/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-23 17:23:52
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-23 17:26:04
 * @FilePath: /epoll_server/core/base/log/logger.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "logger.h"

#include "spdlog/fmt/ostr.h" // must be included

thread_local char err_str[512]{0};

const char *errstr(int err_num)
{
    return strerror_r(err_num, err_str, sizeof(err_str));
}

std::ostream &operator<<(std::ostream &os, const std::thread::id &thread_id)
{
    return os << *static_cast<const unsigned int *>(static_cast<const void *>(&thread_id));
}
