/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-22 14:14:19
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 14:18:30
 * @FilePath: /network-poller/core/net/socket/socket_utility.h
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
            class SocketUtility
            {
            public:
                static bool toAddrIpv4(const std::string &ip, uint16_t port, struct sockaddr_in *addrIn);
                static bool toAddrIpv6(const std::string &ip, uint16_t port, struct sockaddr_in6 *addrIn);

                static int CreateNonblocking(sa_family_t family);

                static void Bind(int sockfd, const struct SocketAddr &addr);
                static void Listen(int sockfd);
                static int Accept(int sockfd, struct SocketAddr *addr);
                static int Connect(int sockfd, const struct SocketAddr &addr);
                static ssize_t Read(int sockfd, void *buf, size_t buf_size);
                static ssize_t ReadV(int sockfd, const struct iovec *iov, int iov_cnt);
                static ssize_t Write(int sockfd, const void *buf, size_t buf_size);
                static void Close(int sockfd);
                static int SocketError(int sockfd);
                static sockaddr_in6 GetPeerAddr(int sockfd);
                static sockaddr_in6 GetLocalAddr(int sockfd);

                static void SetTcpNoDelay(int sockfd, bool on);
                static void SetKeepAlived(int sockfd, bool on);
                static void SetReuseAddr(int sockfd, bool on);
                static void SetReusePort(int sockfd, bool on);
                static void ShutdownWrite(int sockfd);
            };
        }
    }
}