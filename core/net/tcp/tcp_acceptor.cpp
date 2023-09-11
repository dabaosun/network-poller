/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-22 13:54:48
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-29 08:30:41
 * @FilePath: /network-poller/core/net/tcp/tcp_acceptor.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "tcp_acceptor.h"
#include <fcntl.h>
#include <unistd.h>
#include "net/event/event_channel.h"
#include "net/socket/socket_addr.h"
#include "net/socket/socket_utility.h"
#include "base/log/logger.h"

namespace liang
{
    namespace net
    {
        namespace tcp
        {
            TcpAcceptor::TcpAcceptor(EventLoop *loop,
                                     const SocketAddr &addr,
                                     bool resueport) : loop_(loop),
                                                       socket_addr_(addr),
                                                       socket_(SocketUtility::CreateNonblocking(addr.Family())), // creat real fd
                                                       channel_(new EventChannel(loop, socket_.Fd())),           // add fd into channel and assign the channel to loop.
                                                       idle_fd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
            {
                socket_.SetReuseAddr(true);
                socket_.SetReusePort(resueport);
                socket_.Bind(addr);
                channel_->SetReadEventCallBack(std::bind(&TcpAcceptor::AcceptHandle, this)); // register channel read callback to handle new connection
            }

            TcpAcceptor::~TcpAcceptor()
            {
                channel_->DisableAll();     // unregister all event;
                channel_->RemoveFromLoop(); // remove from loop;
                ::close(idle_fd_);
            }

            void TcpAcceptor::Listen()
            {
                assert(loop_->IsInLoopThread());
                LOG_LOG("start listen, addr: {0}, loop: {1}", socket_addr_.ToString(), loop_->Name());
                socket_.Listen();
                channel_->EnableReading(); // wait for new connection.
            }

            void TcpAcceptor::AcceptHandle()
            {
                assert(loop_->IsInLoopThread());
                // handle all new connection until arrive queue empty
                while (1)
                {
                    SocketAddr peer_addr;
                    int connFd = socket_.Accept(&peer_addr);
                    if (connFd >= 0) // new connection
                    {
                        if (new_conn_cb_)
                        {
                            new_conn_cb_(connFd, socket_addr_, peer_addr);
                        }
                        else
                        {
                            // not accept handler, so directly close.
                            SocketUtility::Close(connFd);
                        }
                    }
                    else
                    {
                        if (EAGAIN == errno || EWOULDBLOCK == errno)
                        {
                            break;
                        }
                        int origin_errno = errno;
                        LOG_ERROR("failed to accept , fd: {0}, errno: {1}", socket_.Fd(), origin_errno);
                        if (ENFILE == origin_errno || EMFILE || origin_errno)
                        {
                            ::close(idle_fd_);                         // close /dev/null
                            ::accept(socket_.Fd(), nullptr, nullptr);  // re-accept
                            ::close(idle_fd_);                         // directly close connection
                            ::open("/dev/null", O_RDONLY | O_CLOEXEC); // re-open /dev/null
                        }
                    }
                }
            }
        }
    }
}