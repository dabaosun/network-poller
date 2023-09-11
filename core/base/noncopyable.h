/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-16 17:14:48
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-09-11 12:51:01
 * @FilePath: /network-poller/core/base/noncopyable.h
 * @Description: 
 * 
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

/// @brief define disable copy base class.
namespace liang
{
    namespace base
    {
        class noncopyable
        {
        public:
            noncopyable(const noncopyable &) = delete;
            void operator=(const noncopyable &) = delete;

        protected:
            noncopyable() = default;
            ~noncopyable() = default;
        };
    } // namesapce base
};    // namespace liang