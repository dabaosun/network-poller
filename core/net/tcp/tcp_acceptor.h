/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-28 14:39:42
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 14:39:42
 * @FilePath: /network-poller/core/net/tcp/tcp_acceptor.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "base/noncopyable.h"
#include "net/socket/socket.h"
#include "net/socket/socket_addr.h"
#include "net/event/event_loop.h"
using namespace liang::net::socket;
namespace liang
{
    namespace net
    {
        namespace tcp
        {
            class TcpAcceptor : noncopyable
            {
            public:
                typedef std::function<void(int sockfd, const SocketAddr &local_addr, const SocketAddr &peer_addr)> NewConnectionCB;

                TcpAcceptor(EventLoop *loop, const SocketAddr &addr, bool resueport);
                ~TcpAcceptor();

                void SetNewConnectionCB(const NewConnectionCB &cb)
                {
                    new_conn_cb_ = cb;
                }

            public:
                void Listen();

            private:
                void AcceptHandle();

            private:
                EventLoop *loop_;
                SocketAddr socket_addr_;
                Socket socket_;
                std::unique_ptr<EventChannel> channel_;
                int idle_fd_; // just for ENFILE, EMFILE error when accept.
                NewConnectionCB new_conn_cb_;
            };
        }
    }
}