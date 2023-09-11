/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-21 20:56:28
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 14:37:28
 * @FilePath: /network-poller/core/net/tcp/tcp_server.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "base/noncopyable.h"

#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <atomic>
#include <unordered_map>
#include <functional>

#include "net/socket/socket_addr.h"
#include "base/buffer/buffer.h"

using namespace liang::base;
using namespace liang::net;
using namespace liang::net::socket;

namespace liang
{
    namespace net
    {
        class EventLoop;
        class EventLoopThreadPool;

        namespace tcp
        {
            class TcpAcceptor;
            class TcpConnection;

            class TcpServer : public noncopyable
            {
            public:
                enum Option
                {
                    kNoReusePort,
                    kReusePort,
                };

                TcpServer(EventLoop *loop, SocketAddr &addr, Option option = kReusePort);
                ~TcpServer();

                void SetThreadNum(const uint16_t thread_num);

                void SetMessageCB(std::function<void(std::shared_ptr<TcpConnection> conn, Buffer &buffer)> msg_cb)
                {
                    msg_cb_ = msg_cb;
                }

                void SetWriteCompleteCB(std::function<void(std::shared_ptr<TcpConnection>)> wrote_cb)
                {
                    wrote_cb_ = wrote_cb;
                }

                void SetConnectionCB(std::function<void(std::shared_ptr<TcpConnection>)> connection_cb)
                {
                    connection_cb_ = connection_cb;
                }

                void SetConnStatusCB(std::function<void(std::shared_ptr<TcpConnection>)> conn_status_cb)
                {
                    conn_status_cb_ = conn_status_cb;
                }

                void Start();

            private:
                // it's called back by acceptor when new connection
                void NewConnected(int fd, const SocketAddr &local_addr, const SocketAddr &peer_addr);

                // it's called back by the TcpConnection is closed.
                void ClosedConnection(const std::shared_ptr<TcpConnection> &conn);

            private:
                void RemoveConnection(std::shared_ptr<TcpConnection> conn);

                void RemoveConnectionInLoop(std::shared_ptr<TcpConnection> conn);

            private:
                uint16_t thread_num_;
                std::atomic_bool started_;

                EventLoop *server_loop_;
                std::unique_ptr<EventLoopThreadPool> thread_pool_;

                std::unique_ptr<TcpAcceptor> acceptor_;
                std::unordered_map<int, std::shared_ptr<TcpConnection>> connections_;

                // all functions which recieve the callback from TcpConnection
                std::function<void(std::shared_ptr<TcpConnection> conn, Buffer &buffer)> msg_cb_;
                std::function<void(std::shared_ptr<TcpConnection> conn)> wrote_cb_;
                std::function<void(std::shared_ptr<TcpConnection> conn)> connection_cb_;
                std::function<void(std::shared_ptr<TcpConnection> conn)> close_conn_cb_;
                std::function<void(std::shared_ptr<TcpConnection> conn)> high_water_cb_;
                std::function<void(std::shared_ptr<TcpConnection> conn)> conn_status_cb_;
            };
        }
    }
}