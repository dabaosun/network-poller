/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-27 22:16:08
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 20:44:20
 * @FilePath: /network-poller/core/net/tcp/tcp_client.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include <memory>
#include <functional>
#include "base/noncopyable.h"
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

        namespace tcp
        {
            class TcpConnection;
            class TcpConnector;

            class TcpClient : public noncopyable
            {
            public:
                TcpClient(EventLoop *loop, const SocketAddr &addr);
                ~TcpClient();

            public:
                void Connect();
                void Disconnect();
                void Stop();

                void SetMessageCB(std::function<void(std::shared_ptr<TcpConnection> &conn, Buffer &buffer)> msg_cb)
                {
                    msg_cb_ = msg_cb;
                }

                void SetWriteCompleteCB(std::function<void(std::shared_ptr<TcpConnection> &)> wrote_cb)
                {
                    wrote_cb_ = wrote_cb;
                }

                void SetConnStatusCB(std::function<void(std::shared_ptr<TcpConnection> &)> conn_status_cb)
                {
                    conn_status_cb_ = conn_status_cb;
                }

            private:
                void NewConnection(int fd);

                void RemoveConnection(std::shared_ptr<TcpConnection> &conn);

            private:
                EventLoop *loop_;
                SocketAddr addr_;
                std::unique_ptr<TcpConnector> connector_;
                std::shared_ptr<TcpConnection> connection_;

                // all functions which recieve the callback from TcpConnection
                std::function<void(std::shared_ptr<TcpConnection> &conn, Buffer &buffer)> msg_cb_;
                std::function<void(std::shared_ptr<TcpConnection> &conn)> wrote_cb_;
                std::function<void(std::shared_ptr<TcpConnection> &conn)> close_conn_cb_;
                std::function<void(std::shared_ptr<TcpConnection> &conn)> high_water_cb_;
                std::function<void(std::shared_ptr<TcpConnection> &conn)> conn_status_cb_;
            };
        }
    }

} // namespace liang
