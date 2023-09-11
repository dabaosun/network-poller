/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-28 10:04:53
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 22:11:46
 * @FilePath: /network-poller/core/net/tcp/tcp_connector.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <memory>
#include <functional>

#include "base/noncopyable.h"
#include "net/socket/socket_addr.h"
#include "net/event/event_channel.h"

using namespace liang::net::socket;

namespace liang
{
    namespace net
    {
        class EventLoop;
        class EventChannel;

        namespace tcp
        {
            class TcpConnector : public noncopyable
            {
            public:
                enum States
                {
                    kDisconnected,
                    kConnecting,
                    kConnected
                };

                TcpConnector(EventLoop *loop, const SocketAddr &addr)
                    : loop_(loop),
                      addr_(addr),
                      state_(kDisconnected)
                {
                }

                void SetNewConnectionCB(std::function<void(int sockfd)> cb)
                {
                    new_connection_cb_ = cb;
                }

            public:
                void Start();
                void Stop();

            private:
                void StartInLoop();
                void StopInLoop();

            private:
                void RetryConn(int fd);

            private:
                void WriteEventHandler();
                void ErrorEventHandler();

            private:
             int RemoveChannel();
             void ResetChannelPtr();
             
            private:
                EventLoop *loop_;
                SocketAddr addr_;
                std::atomic<States> state_;

                std::unique_ptr<EventChannel> channel_;
                std::function<void(int sockfd)> new_connection_cb_;
            };
        }
    }
}