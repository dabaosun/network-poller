/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-22 14:17:37
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-22 15:02:45
 * @FilePath: /network-poller/core/net/socket/socket.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "socket.h"
#include "net/socket/socket_utility.h"

namespace liang
{
    namespace net
    {
        namespace socket
        {
            Socket::~Socket()
            {
                SocketUtility::Close(socket_fd_);
            }

            void Socket::Bind(const SocketAddr &addr)
            {
                return SocketUtility::Bind(socket_fd_, addr);
            }

            void Socket::Listen()
            {
                return SocketUtility::Listen(socket_fd_);
            }

            int Socket::Accept(SocketAddr *addr)
            {
                return SocketUtility::Accept(socket_fd_, addr);
            }

            void Socket::SetTcpNoDelay(bool on)
            {
                return SocketUtility::SetTcpNoDelay(socket_fd_, on);
            }

            void Socket::SetKeepAlived(bool on)
            {
                return SocketUtility::SetKeepAlived(socket_fd_, on);
            }

            void Socket::SetReuseAddr(bool on)
            {
                return SocketUtility::SetReuseAddr(socket_fd_, on);
            }

            void Socket::SetReusePort(bool on)
            {
                return SocketUtility::SetReusePort(socket_fd_, on);
            }

            void Socket::ShutdownWrite()
            {
                SocketUtility::ShutdownWrite(this->socket_fd_);
            }
        }
    }
}