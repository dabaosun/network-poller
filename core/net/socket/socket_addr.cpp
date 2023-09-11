/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-21 22:19:17
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-26 12:37:39
 * @FilePath: /network-poller/core/net/socket/socket_addr.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "socket_addr.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "net/socket/socket_utility.h"

namespace liang
{
    namespace net
    {
        namespace socket
        {
            SocketAddr::SocketAddr(const std::string &ip, uint16_t port, bool ipv6)
            {
                if (ipv6)
                {
                    SocketUtility::toAddrIpv6(ip, port, &sock_addr6_);
                }
                else
                {
                    SocketUtility::toAddrIpv4(ip, port, &sock_addr4_);
                }
            }

            SocketAddr::~SocketAddr()
            {
            }

            std::string SocketAddr::ToString() const
            {
                char buf[64]{0};
                sockaddr *sock_addr = static_cast<sockaddr *>((void *)&sock_addr6_);
                if (sock_addr->sa_family == AF_INET6)
                {
                    int addr_size = sizeof(sock_addr6_);
                    ::inet_ntop(AF_INET6, (void *)&sock_addr6_.sin6_addr, buf, static_cast<socklen_t>(sizeof(buf)));
                    uint16_t port = ::ntohs(sock_addr6_.sin6_port);
                    snprintf(buf + ::strlen(buf), sizeof(buf) - ::strlen(buf), "%d", port);
                }
                else
                {
                    int addr_size = sizeof(sock_addr4_);
                    ::inet_ntop(AF_INET, (void *)&sock_addr4_.sin_addr, buf, static_cast<socklen_t>(sizeof(buf)));
                    uint16_t port = ::ntohs(sock_addr4_.sin_port);
                    snprintf(buf + ::strlen(buf), sizeof(buf) - ::strlen(buf), ":%d", port);
                }
                return buf;
            }
        }
    }
}
