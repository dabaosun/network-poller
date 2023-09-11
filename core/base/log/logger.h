/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-20 14:17:18
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-24 15:17:49
 * @FilePath: /network-poller/core/base/log/logger.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include <thread>

const char *errstr(int err_num);

extern std::ostream &operator<<(std::ostream &os, const std::thread::id &thread_id);

#define LEVEL_TRACE spdlog::level::level_enum::trace
#define LEVEL_DEBUG spdlog::level::level_enum::debug
#define LEVEL_INFO spdlog::level::level_enum::info
#define LEVEL_WARN spdlog::level::level_enum::warn
#define LEVEL_ERROR spdlog::level::level_enum::err
#define LEVEL_CRITICAL spdlog::level::level_enum::critical
#define LEVEL_OFF spdlog::level::level_enum::n_levels

#define SET_LOG_LEVEL(level)  \
    spdlog::set_level(level); \
    spdlog::set_pattern("[%H:%M:%S.%e] [%P] [%t] [%s:%#] %v")

#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_LOG(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
