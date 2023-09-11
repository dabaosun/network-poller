/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-22 08:19:16
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 14:11:33
 * @FilePath: /network-poller/core/net/socket/socket_addr.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <string>
#include <netinet/in.h>

namespace liang
{
    namespace net
    {
        namespace socket
        {
            // utility classs for Internet socket address
            class SocketAddr
            {
            public:
                SocketAddr()
                {
                }
                SocketAddr(const std::string &ip, uint16_t port, bool ipv6);

                SocketAddr(const struct sockaddr_in &addr) : sock_addr4_(addr)
                {
                }

                SocketAddr(const struct sockaddr_in6 &addr) : sock_addr6_(addr)
                {
                }

                ~SocketAddr();

                void SetSocketAddrInet(const struct sockaddr_in6 &addr6)
                {
                    sock_addr6_ = addr6;
                }

                const struct sockaddr *GetSockAddr() const
                {
                    return static_cast<struct sockaddr *>((void *)&sock_addr6_);
                }

                std::string ToString() const;
                sa_family_t Family() const
                {
                    return sock_addr4_.sin_family;
                }

            private:
                union
                {
                    struct sockaddr_in sock_addr4_;
                    struct sockaddr_in6 sock_addr6_;
                };
            };
        }
    }
}