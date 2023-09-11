/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-28 20:44:55
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-09-02 11:30:48
 * @FilePath: /network-poller/core/net/tests/echo_client_test.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <vector>
#include <sstream>

#include "net/event/event_loop.h"
#include "net/tcp/tcp_client.h"
#include "net/tcp/tcp_connection.h"
#include "net/socket/socket_addr.h"
#include "base/log/logger.h"

using namespace liang::net::tcp;
using namespace liang::net;
using namespace liang::base;

class EchoClient;

uint current = 0;
std::vector<std::unique_ptr<EchoClient>> clients;

class EchoClient
{
public:
    EchoClient(EventLoop *loop, const SocketAddr &remote_addr)
        : loop_(loop),
          tcp_client_(loop, remote_addr)
    {
        tcp_client_.SetConnStatusCB(std::bind(&EchoClient::OnConnStatusCB, this, std::placeholders::_1));
        tcp_client_.SetMessageCB(std::bind(&EchoClient::OnMessageCB, this, std::placeholders::_1, std::placeholders::_2));
        tcp_client_.SetWriteCompleteCB(std::bind(&EchoClient::OnWriteCompleteCB, this, std::placeholders::_1));
    }

public:
    void Connect()
    {
        tcp_client_.Connect();
    }

private:
    void OnConnStatusCB(std::shared_ptr<TcpConnection> &conn)
    {
        LOG_DEBUG("connection status changed, name: {0}, state: {1}", conn->Name(), conn->State());
        if (conn->State() == TcpConnection::ConnState::CONNECTED)
        {
            ++current;
            if (static_cast<int>(clients.size()) > current)
            {
                clients[current]->Connect();
            }
        }

        auto deletor = [](uint8_t *p)
        {
            delete[] p;
        };
        std::shared_ptr<uint8_t> send_msg(new uint8_t[6]{'w', 'o', 'r', 'l', 'd', '\n'}, deletor);
        conn->Send(send_msg, strlen("world\n"));
    }

    void OnMessageCB(std::shared_ptr<TcpConnection> &conn, Buffer &buffer)
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

    void OnWriteCompleteCB(std::shared_ptr<TcpConnection> &conn)
    {
    }

private:
    EventLoop *loop_;
    TcpClient tcp_client_;
};

int main(int argc, char **args)
{
    std::ostringstream ss;

    ss << std::this_thread::get_id();

    std::string idstr = ss.str();

    SET_LOG_LEVEL(LEVEL_DEBUG);
    // ip port count is_ipv6LEVEL_DEBUG
    // if (argc >= 5)
    {
        // std::string ip(args[1]);
        // int port = atoi(args[2]);
        // uint cnt = atoi(args[3]);
        // bool ip_v6 = atoi(args[4]);

        std::string ip("127.0.0.1");
        int port = 1080;
        uint cnt = 100000;
        bool ip_v6 = false;

        EventLoop loop("echo_client_loop");
        SocketAddr server_addr(ip, port, ip_v6);
        clients.reserve(cnt);
        for (int i = 0; i < cnt; i++)
        {
            clients.emplace_back(new EchoClient(&loop, server_addr));
        }
        clients[current]->Connect();
        loop.Loop();
    }
}
