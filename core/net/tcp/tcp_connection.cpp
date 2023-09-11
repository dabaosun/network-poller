/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-23 16:34:29
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-29 08:54:18
 * @FilePath: /epoll_server/core/net/tcp/tcp_connection.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause 
 */
#include "tcp_connection.h"

#include "net/socket/socket.h"
#include "net/event/event_channel.h"
#include "base/log/logger.h"
#include "net/socket/socket_utility.h"

namespace liang
{
    namespace net
    {
        namespace tcp
        {
            TcpConnection::TcpConnection(int fd, EventLoop *loop,
                                         const SocketAddr &local_addr,
                                         const SocketAddr &peer_addr)
                : name_(local_addr.ToString() + "_" + peer_addr.ToString()),
                  loop_(loop),
                  socket_(new Socket(fd)),
                  channel_(new EventChannel(loop, fd)),
                  fd_(fd)

            {
                // register the callback func on channel.
                channel_->SetCloseEventCallBack(std::bind(&TcpConnection::CloseHandler, this));
                channel_->SetErrorEventCallBack(std::bind(&TcpConnection::ErrorHandler, this));
                channel_->SetReadEventCallBack(std::bind(&TcpConnection::ReadHandler, this));
                channel_->SetWriteEventCallBack(std::bind(&TcpConnection::WriteHandler, this));
                // channel_->EnableReading();
                socket_->SetKeepAlived(true);
                LOG_DEBUG("connection new, name: {0}, fd: {1}", name_, fd_);
            }

            TcpConnection::~TcpConnection()
            {
            }

            int TcpConnection::Fd() const
            {
                return fd_;
            }

            void TcpConnection::Send(const std::shared_ptr<uint8_t> &msg, const int msg_size)
            {
                if (loop_->IsInLoopThread())
                {
                    SendInLoop(msg, msg_size);
                }
                else
                {
                    loop_->QueueInLoop(std::bind(&TcpConnection::SendInLoop, shared_from_this(), msg, msg_size));
                }
            }

            void TcpConnection::StartRead()
            {
                loop_->QueueInLoop(std::bind(&TcpConnection::StartReadInLoop, shared_from_this()));
            }

            void TcpConnection::StopRead()
            {
                loop_->QueueInLoop(std::bind(&TcpConnection::StopReadInLoop, shared_from_this()));
            }

            void TcpConnection::Shutdown()
            {
                ConnState expected_state = CONNECTED;
                if (state_.compare_exchange_weak(expected_state, DISCONNECTING, std::memory_order_relaxed))
                {
                    loop_->QueueInLoop(std::bind(&TcpConnection::ShutdownInLoop, shared_from_this()));
                }
            }

            void TcpConnection::ForceClose(double seconds)
            {
                ConnState expected_state = CONNECTED;
                if (state_.compare_exchange_weak(expected_state, DISCONNECTING, std::memory_order_relaxed))
                {
                    loop_->QueueInLoop(std::bind(&TcpConnection::ForceCloseInLoop, shared_from_this(), seconds));
                }
                expected_state = DISCONNECTING;
                if (state_.compare_exchange_weak(expected_state, DISCONNECTING, std::memory_order_relaxed))
                {
                    loop_->QueueInLoop(std::bind(&TcpConnection::ForceCloseInLoop, shared_from_this(), seconds));
                }
            }

            void TcpConnection::ConnectEstablished()
            {
                assert(loop_->IsInLoopThread());
                SetState(CONNECTED);
                channel_->EnableReading();
                if (conn_status_cb_)
                {
                    auto conn_ptr = shared_from_this();
                    conn_status_cb_(conn_ptr);
                }
            }

            void TcpConnection::ConnectDestoryed()
            {
                assert(loop_->IsInLoopThread());
                LOG_LOG("connection destoryed, name: {0}, fd: {1}", name_, fd_);
                if (CONNECTED == state_)
                {
                    SetState(DISCONNECTED);
                    channel_->DisableAll();
                    if (conn_status_cb_)
                    {
                        auto conn_ptr = shared_from_this();
                        conn_status_cb_(conn_ptr);
                    }
                }
                channel_->RemoveFromLoop(); // remove from loop
            }

            void TcpConnection::ErrorHandler()
            {
                int err = SocketUtility::SocketError(fd_);
                LOG_ERROR("connection socket err, name: {0}, fd: {1}, errno: {2}, err_descr: {3}", name_, fd_, err, errstr(err));
                // if ((err != EWOULDBLOCK) || (err != EAGAIN))
                // {
                //     switch (err)
                //     {
                //     case EPIPE:
                //     case ECONNRESET:
                //         // connection is aborted
                //         CloseHandler();
                //     default:
                //         break;
                //     }
                // }
            }

            void TcpConnection::CloseHandler()
            {
                assert(loop_->IsInLoopThread());
                LOG_LOG("connection closed, name: {0}, fd: {1}", name_, fd_);
                SetState(DISCONNECTED);
                channel_->DisableAll(); // set none event on poll
                if (conn_status_cb_)
                {
                    auto conn_ptr = shared_from_this();
                    conn_status_cb_(conn_ptr);
                }

                assert(close_cb_); // the call back must exist which to release/close connection socket.
                auto conn_ptr = shared_from_this();
                close_cb_(conn_ptr);
            }

            void TcpConnection::ReadHandler()
            {
                assert(loop_->IsInLoopThread());

                while (1)
                {
                    int readErr;
                    int len = read_buffer_.ReadFromFD(this->channel_->Fd(), readErr);
                    if (len > 0)
                    {
                        LOG_TRACE("tcp connection received data, len: {0}", len);
                        // read successfully
                        if (msg_cb_)
                        {
                            LOG_TRACE("tcp connection received data, callback: {0}, readablebytes: {1}", shared_from_this(), read_buffer_.ReadableBytes());
                            // call back
                            auto conn_ptr = shared_from_this();
                            msg_cb_(conn_ptr, read_buffer_);
                        }
                        else
                        {
                            LOG_WARN("no message callback");
                            // directly close connection.
                            CloseHandler();
                            break;
                        }
                    }
                    else if (0 == len)
                    {
                        // EOF
                        LOG_TRACE("connection read EOF, fd: {0}", channel_->Fd());
                        CloseHandler();
                        break;
                    }
                    else
                    {
                        if (readErr == EAGAIN)
                        {
                            break;
                        }
                        errno = readErr; // recovert errno
                        LOG_ERROR("connection read err, fd: {0}, errno: {1}", channel_->Fd(), errno);
                        ErrorHandler();
                        break;
                    }
                }
            }
            void TcpConnection::WriteHandler()
            {
                assert(loop_->IsInLoopThread());

                if (channel_->IsWriting())
                {
                    // kernel buffer writable
                    ssize_t n_size = SocketUtility::Write(channel_->Fd(), write_buffer_.ReadIndexPtr(), write_buffer_.ReadableBytes());
                    if (n_size > 0)
                    {
                        // write to kernel buffer successfully
                        write_buffer_.ClearReadIndex(n_size); // move read index for next read
                        if (write_buffer_.IsEmpty())
                        {
                            channel_->DisableWriting(); // temporaliy disable write event since all of write_buffer are handled.
                            if (written_cb_)
                            {
                                // notify write completion.
                                loop_->RunInLoop(std::bind(written_cb_, shared_from_this()));
                            }
                            if (DISCONNECTING == state_)
                            {
                                // disconnecting in progress
                                loop_->RunInLoop(std::bind(&TcpConnection::ShutdownInLoop, this));
                            }
                        }
                    }
                    else
                    {
                        LOG_ERROR("write handler error, fd: {0}, errno: {1}", channel_->Fd(), errno);
                    }
                }
            }

            void TcpConnection::ShutdownInLoop()
            {
                assert(loop_->IsInLoopThread());
                if (!channel_->IsWriting())
                {
                    socket_->ShutdownWrite();
                }
            }

            void TcpConnection::ForceCloseInLoop(double seconds)
            {
                assert(loop_->IsInLoopThread());
                LOG_DEBUG("force close, name: {0}, fd: {1}", name_, fd_);
                if ((CONNECTED == state_) || (DISCONNECTING == state_))
                {
                    CloseHandler();
                }
            }

            void TcpConnection::SendInLoop(const std::shared_ptr<uint8_t> msg, const int msg_size)
            {
                assert(loop_->IsInLoopThread());

                if (DISCONNECTED == state_)
                {
                    // have been diconnected, directly return
                    LOG_WARN("disconnected, discard write data");
                    return;
                }

                ssize_t nwrite = 0;
                size_t remaining = msg_size;
                bool faultErr = false;
                if (!channel_->IsWriting() && write_buffer_.ReadableBytes() == 0) // channel not working on write event
                {
                    // try to write directly, two aims: A.send data. B.check the connection state.
                    nwrite = SocketUtility::Write(channel_->Fd(), (const void *)msg.get(), msg_size);
                    if (nwrite >= 0)
                    {
                        // write succesfully
                        remaining = remaining - nwrite;
                        if ((0 == remaining) && (written_cb_))
                        {
                            loop_->QueueInLoop(std::bind(written_cb_, shared_from_this()));
                        }
                    }
                    else
                    { // error happened
                        nwrite = 0;
                        if ((errno != EWOULDBLOCK) || (errno != EAGAIN))
                        {
                            LOG_ERROR("write failed, system error happened, fd: {0}, errno: {1}", channel_->Fd(), errno);
                            switch (errno)
                            {
                            case EPIPE:
                            case ECONNRESET:
                                // connection is aborted
                                faultErr = true;
                            default:
                                break;
                            }
                        }
                    }
                }

                // two cases: A. remain data which not wrote. B. fault error happened.
                if (faultErr)
                {
                    // since fault error happend, close it.
                    CloseHandler();
                }

                if (!faultErr && (remaining > 0))
                {
                    size_t readable_len = write_buffer_.ReadableBytes();
                    static size_t highWaterMark_ = 6 * 1024 * 1024;

                    if (readable_len + remaining > highWaterMark_)
                    {
                        if (high_water_cb_)
                        {
                            loop_->QueueInLoop(std::bind(high_water_cb_, shared_from_this(), high_water_));
                        }
                    }
                    write_buffer_.Append((const char *)(char *)(msg.get() + nwrite), remaining);
                    if (!channel_->IsWriting())
                    {
                        channel_->EnableWriting(); // add write event to poll
                    }
                }
            }

            void TcpConnection::StartReadInLoop()
            {
                assert(loop_->IsInLoopThread());
                if (!channel_->IsReading())
                {
                    channel_->EnableReading();
                }
            }

            void TcpConnection::StopReadInLoop()
            {
                assert(loop_->IsInLoopThread());

                if (channel_->IsReading())
                {
                    channel_->DisableReading();
                }
            }
        }
    }
}