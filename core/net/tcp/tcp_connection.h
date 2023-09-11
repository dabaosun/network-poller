/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-23 16:57:54
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 20:56:18
 * @FilePath: /network-poller/core/net/tcp/tcp_connection.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#pragma once

#include <memory>
#include <atomic>
#include <functional>

#include "base/noncopyable.h"
#include "base/buffer/buffer.h"
#include "net/socket/socket.h"
#include "net/socket/socket_addr.h"

using namespace liang::base;
using namespace liang::net::socket;

namespace liang
{
    namespace net
    {
        class EventLoop;
        class EventChannel;

        namespace tcp
        {

            class TcpConnection : public std::enable_shared_from_this<TcpConnection>
            {
            public:
                using OnMessageCallback = std::function<void(std::shared_ptr<TcpConnection> &, Buffer &)>;
                using OnCloseCallback = std::function<void(std::shared_ptr<TcpConnection> &)>;
                using OnWriteCompletCallback = std::function<void(std::shared_ptr<TcpConnection> &)>;
                using OnHighWaterMarkCallback = std::function<void(std::shared_ptr<TcpConnection> &, size_t)>;
                using OnConnStatusCallback = std::function<void(std::shared_ptr<TcpConnection> &)>;

                enum ConnState
                {
                    CONNECTING,
                    CONNECTED,
                    DISCONNECTING,
                    DISCONNECTED,

                };

            public:
                TcpConnection(int fd_, EventLoop *loop, const SocketAddr &local_addr, const SocketAddr &peer_addr);
                ~TcpConnection();

                int Fd() const;

                EventLoop *Loop() const
                {
                    return loop_;
                }

                std::string Name() const
                {
                    return name_;
                }

                ConnState State() const
                {
                    return state_;
                }

                void SetMessagCB(OnMessageCallback cb)
                {
                    msg_cb_ = cb;
                }

                void SetCloseCB(OnCloseCallback cb)
                {
                    close_cb_ = cb;
                }

                void SetWriteCompleteCB(OnWriteCompletCallback cb)
                {
                    written_cb_ = cb;
                }

                void SetHighWaterCB(OnHighWaterMarkCallback cb, size_t high_water)
                {
                    high_water_cb_ = cb;
                    high_water_ = high_water;
                }

                void SetConnStatusCB(OnConnStatusCallback cb)
                {
                    conn_status_cb_ = cb;
                }
                void SetState(ConnState state)
                {
                    state_ = state;
                }

            public: // all functions are thread safe for upper layer caller.
                void Send(const std::shared_ptr<uint8_t> &msg, const int msg_size);
                void StartRead();
                void StopRead();
                void Shutdown();
                void ForceClose(double seconds = 0);

                void ConnectEstablished();
                void ConnectDestoryed();

            private:
                // all functions called back by TcpChannel
                void ErrorHandler();
                void CloseHandler();
                void ReadHandler();
                void WriteHandler();

            private:
                void SendInLoop(const std::shared_ptr<uint8_t> msg, const int msg_size); // handle @Send() in loop
                void StartReadInLoop();                                                  // handle @StartRed() in loop
                void StopReadInLoop();                                                   // handle @StopRead() in loop
                void ShutdownInLoop();
                void ForceCloseInLoop(double second); // handle @ForceClose() in loop

            private:
                std::string name_; // TODO: remote_port_local_port
                std::atomic<ConnState> state_{CONNECTING};
                EventLoop *loop_;
                std::unique_ptr<Socket> socket_;
                std::unique_ptr<EventChannel> channel_;
                int fd_;

                base::Buffer read_buffer_;
                base::Buffer write_buffer_;
                size_t high_water_ = 6 * 1024 * 1024;

                // all callbacks function which TcpConnection loop will call.
                // normally the callback reciever is TcpServer.
                OnMessageCallback msg_cb_;              // will called when connection read data.
                OnCloseCallback close_cb_;              // will called when connection closed
                OnWriteCompletCallback written_cb_;     // will called when connection write complete
                OnHighWaterMarkCallback high_water_cb_; // will called when connection in high water mark
                OnConnStatusCallback conn_status_cb_;   // will called when connection state changed
            };

        } // namespace tcp

    }
}