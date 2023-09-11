/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-22 10:37:18
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-22 15:04:45
 * @FilePath: /network-poller/core/net/socket/socket.h
 * @Description: ;
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#pragma once

#include "net/socket/socket_addr.h"

namespace liang
{
    namespace net
    {
        namespace socket
        {
            class Socket
            {
            public:
                Socket(int fd) : socket_fd_(fd){

                                 };
                ~Socket();
                void Bind(const SocketAddr &addr);
                void Listen();
                int Accept(SocketAddr *addr);

                void SetTcpNoDelay(bool on);
                void SetKeepAlived(bool on);
                void SetReuseAddr(bool on);
                void SetReusePort(bool on);
                void ShutdownWrite();

                int Fd() {
                    return socket_fd_;
                }

            private:
                int socket_fd_;
            };
        }
    }
}