/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-22 09:41:35
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-09-01 10:41:54
 * @FilePath: /network-poller/core/net/tcp/tcp_server.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "tcp_server.h"

#include <functional>

#include "net/event/event_loop.h"
#include "net/event/event_loop_thread_pool.h"
#include "net/event/event_channel.h"
#include "net/tcp/tcp_acceptor.h"
#include "net/tcp/tcp_connection.h"

#include "base/log/logger.h"

namespace liang
{
    namespace net
    {
        namespace tcp
        {
            TcpServer::TcpServer(EventLoop *loop, SocketAddr &addr, Option option) : thread_num_(0),
                                                                                     started_(false),
                                                                                     server_loop_(loop),
                                                                                     acceptor_(new TcpAcceptor(loop, addr, kReusePort == option))
            {

                // set new connection arrvied callback
                acceptor_->SetNewConnectionCB(std::bind(&TcpServer::NewConnected, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

                close_conn_cb_ = std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1);
            }

            TcpServer::~TcpServer()
            {
            }

            void TcpServer::SetThreadNum(const uint16_t thread_num)
            {
                if (thread_num > std::thread::hardware_concurrency())
                {
                    this->thread_num_ = std::thread::hardware_concurrency();
                }
                else
                {
                    this->thread_num_ = thread_num;
                }
            }

            void TcpServer::Start()
            {
                bool flag = false;
                if (started_.compare_exchange_weak(flag, true, std::memory_order_relaxed))
                {
                    // initialize thread pool.
                    thread_pool_ = std::unique_ptr<EventLoopThreadPool>(new EventLoopThreadPool("tcpserver", thread_num_, server_loop_));
                    thread_pool_->Initialize();

                    // acceptor start listen
                    server_loop_->QueueInLoop(std::bind(&TcpAcceptor::Listen, acceptor_.get()));
                }
                return;
            }

            void TcpServer::NewConnected(int fd, const SocketAddr &local_addr, const SocketAddr &peer_addr)
            {
                assert(server_loop_->IsInLoopThread());
                EventLoop *work_pool = thread_pool_->GetEventLoopFromPoolForHash(fd);
                std::shared_ptr<TcpConnection> conn_new = std::make_shared<TcpConnection>(fd, work_pool, local_addr, peer_addr);
                connections_[fd] = conn_new;
                conn_new->SetCloseCB(close_conn_cb_);
                conn_new->SetWriteCompleteCB(wrote_cb_);
                conn_new->SetConnStatusCB(conn_status_cb_);
                conn_new->SetMessagCB(msg_cb_);
                conn_new->SetWriteCompleteCB(wrote_cb_);
                conn_new->SetWriteCompleteCB(high_water_cb_);
                LOG_DEBUG("new connection, fd: {0}, local_addr: {1}, peer_addr: {2}, loop: {3}",
                          fd,
                          local_addr.ToString(),
                          peer_addr.ToString(),
                          work_pool->Name());
                work_pool->QueueInLoop(std::bind(&TcpConnection::ConnectEstablished, conn_new)); // trigger the work loop to recieve the data.
            }

            void TcpServer::ClosedConnection(const std::shared_ptr<TcpConnection> &conn)
            {
                if (server_loop_->IsInLoopThread())
                {
                    this->RemoveConnectionInLoop(conn);
                }
                else
                {
                    server_loop_->QueueInLoop(std::bind(&TcpServer::RemoveConnectionInLoop, this, conn));
                }
            }

            void TcpServer::RemoveConnection(std::shared_ptr<TcpConnection> conn)
            {
                if (server_loop_->IsInLoopThread())
                {
                    RemoveConnectionInLoop(conn);
                }
                else
                {
                    server_loop_->QueueInLoop(std::bind(&TcpServer::RemoveConnectionInLoop, this, conn));
                }
            }

            void TcpServer::RemoveConnectionInLoop(std::shared_ptr<TcpConnection> conn)
            {
                assert(server_loop_->IsInLoopThread());
                LOG_DEBUG("remove connection, fd: {0}", conn->Fd());
                connections_.erase(conn->Fd());
                EventLoop *loop = conn->Loop();
                loop->QueueInLoop(std::bind(&TcpConnection::ConnectDestoryed, conn));
            }
        }
    }
}