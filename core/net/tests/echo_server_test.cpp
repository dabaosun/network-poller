/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-24 12:24:35
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-29 08:46:38
 * @FilePath: /network-poller/core/net/tests/echo_server_test.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "net/socket/socket_addr.h"
#include "net/tcp/tcp_server.h"
#include "net/tcp/tcp_connection.h"
#include "net/event/event_loop.h"
#include "base/log/logger.h"
#include "net/event/event_loop_thread_pool.h"

#include <memory>

using namespace liang::net::tcp;
using namespace liang::net;
using namespace liang::base;

class EchoServer
{

public:
    EchoServer(EventLoop *loop, SocketAddr &addr) : loop_(loop),
                                                    server_(loop, addr)
    {

        server_.SetThreadNum(16);
        buffer_.reserve(65536);
    };

    ~EchoServer();

    void Start();

public:
    void OnMessageCallback(std::shared_ptr<TcpConnection> conn, Buffer &buffer); // connection read data
    void OnWriteCompletCallback(std::shared_ptr<TcpConnection> conn);            // connection write complete
    void OnConnectionCallback(std::shared_ptr<TcpConnection> conn);              // connection established
    void OnCloseConnCallback(std::shared_ptr<TcpConnection> conn);               // connection closed.
    void OnHighwaterMarkerCallback(std::shared_ptr<TcpConnection> conn);         // connection in high water marker
    void OnConnStatusCallback(std::shared_ptr<TcpConnection> conn);              // connection status changed

private:
    EventLoop *loop_;
    TcpServer server_;
    std::vector<uint8_t> buffer_;
};

int main(int args, char **argv)
{
    SET_LOG_LEVEL(LEVEL_INFO);
    LOG_LOG("echo server, pid: {0}, tid: {1}", ::getpid(), std::this_thread::get_id());
    EventLoop main_loop("echo_server_main_loop");
    SocketAddr addr(std::string("0.0.0.0"), 1080, false);
    std::string str = addr.ToString();
    EchoServer echo_server(&main_loop, addr);
    echo_server.Start();
    main_loop.Loop();

    int i = 0;
}

EchoServer::~EchoServer()
{
}

void EchoServer::Start()
{
    server_.SetConnStatusCB(std::bind(&EchoServer::OnConnStatusCallback, this, std::placeholders::_1));
    server_.SetMessageCB(std::bind(&EchoServer::OnMessageCallback, this, std::placeholders::_1, std::placeholders::_2));
    server_.SetConnectionCB(std::bind(&EchoServer::OnConnectionCallback, this, std::placeholders::_1));
    server_.SetWriteCompleteCB(std::bind(&EchoServer::OnWriteCompletCallback, this, std::placeholders::_1));
    server_.Start();
}

void EchoServer::OnMessageCallback(std::shared_ptr<TcpConnection> conn, Buffer &buffer)
{
    std::vector<uint8_t> msg;
    int read_len = buffer.ReadAllAsByte(msg);
    if ((msg.size() <= 6) && (0 == memcmp(msg.data(), "exit", strlen("exit"))))
    {
        auto deletor = [](uint8_t *p)
        {
            delete[] p;
        };
        std::shared_ptr<uint8_t> send_msg(new uint8_t[4]{'b', 'y', 'e', '\n'}, deletor);
        conn->Send(send_msg, strlen("bye\n"));
        conn->Shutdown();
    }

    if ((msg.size() <= 6) && (0 == memcmp(msg.data(), "quit", strlen("quit"))))
    {
        loop_->Quit();
    }

    auto deletor = [](uint8_t *p)
    {
        delete[] p;
    };
    std::shared_ptr<uint8_t> send_msg(new uint8_t[read_len], deletor);
    memcpy(send_msg.get(), &*msg.begin(), read_len);
    conn->Send(send_msg, read_len);
}

void EchoServer::OnWriteCompletCallback(std::shared_ptr<TcpConnection> conn)
{
    LOG_DEBUG("write complete");
}

void EchoServer::OnConnectionCallback(std::shared_ptr<TcpConnection> conn)
{
    LOG_DEBUG("connection");
}

void EchoServer::OnCloseConnCallback(std::shared_ptr<TcpConnection> conn)
{
    LOG_DEBUG("close");
}

void EchoServer::OnHighwaterMarkerCallback(std::shared_ptr<TcpConnection> conn)
{
    LOG_DEBUG("high water mark");
}

void EchoServer::OnConnStatusCallback(std::shared_ptr<TcpConnection> conn)
{
    LOG_DEBUG("status changed, status: {0}", conn->State());
}
