/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-27 22:24:16
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 20:38:20
 * @FilePath: /network-poller/core/net/tcp/tcp_client.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "tcp_client.h"
#include <assert.h>
#include "net/tcp/tcp_connector.h"
#include "net/tcp/tcp_connection.h"
#include "net/socket/socket_utility.h"
#include "net/event/event_loop.h"
#include "base/log/logger.h"

using namespace liang::net::tcp;

namespace liang
{
    namespace net
    {
        namespace tcp
        {
            void RemoveConnection(EventLoop *loop, const std::shared_ptr<TcpConnection> &conn)
            {
                loop->QueueInLoop(std::bind(&TcpConnection::ConnectDestoryed, conn));
            }

            TcpClient::TcpClient(EventLoop *loop, const SocketAddr &addr)
                : loop_(loop),
                  addr_(addr),
                  connector_(new TcpConnector(loop, addr))
            {
                connector_->SetNewConnectionCB(std::bind(&TcpClient::NewConnection,this,std::placeholders::_1));
            }

            TcpClient::~TcpClient()
            {
                if (connection_)
                {
                    std::function<void(std::shared_ptr<TcpConnection> &)> cb = std::bind(&tcp::RemoveConnection,
                                                                                         loop_,
                                                                                         std::placeholders::_1);
                    loop_->RunInLoop(std::bind(&TcpConnection::SetCloseCB,
                                               connection_, cb));
                }
            }

            void TcpClient::Connect()
            {
                connector_->Start();
            }

            void TcpClient::Disconnect()
            {
                if (connection_)
                {
                    connection_->Shutdown();
                }
            }

            void TcpClient::Stop()
            {
                connector_->Stop();
            }

            void TcpClient::NewConnection(int fd)
            {
                assert(loop_->IsInLoopThread());
                SocketAddr peer_addr(SocketUtility::GetPeerAddr(fd));
                SocketAddr local_addr(SocketUtility::GetLocalAddr(fd));

                connection_ = std::make_shared<TcpConnection>(fd, loop_, local_addr, peer_addr);
                connection_->SetConnStatusCB(conn_status_cb_);
                connection_->SetMessagCB(msg_cb_);
                connection_->SetWriteCompleteCB(wrote_cb_);
                connection_->SetCloseCB(std::bind(&TcpClient::RemoveConnection, this, std::placeholders::_1));

                connection_->ConnectEstablished();
            }

            void TcpClient::RemoveConnection(std::shared_ptr<TcpConnection> &conn)
            {
                assert(loop_->IsInLoopThread());
                assert(loop_ == conn->Loop());
                connection_.reset();
                loop_->QueueInLoop(std::bind(&TcpConnection::ConnectDestoryed, conn));
            }
        }
    }
}
