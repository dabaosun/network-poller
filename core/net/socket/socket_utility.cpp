/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-22 08:43:44
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 21:16:17
 * @FilePath: /network-poller/core/net/socket/socket_utility.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "socket_utility.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/uio.h>

#include "base/log/logger.h"
#include "net/socket/socket_addr.h"

namespace liang
{
    namespace net
    {
        namespace socket
        {
            bool SocketUtility::toAddrIpv4(const std::string &ip, uint16_t port, sockaddr_in *addrIn)
            {
                addrIn->sin_family = AF_INET;
                addrIn->sin_port = htons(port);
                if (::inet_pton(AF_INET, ip.c_str(), &addrIn->sin_addr) == 1)
                {
                    return true;
                }

                return false;
            }

            bool SocketUtility::toAddrIpv6(const std::string &ip, uint16_t port, sockaddr_in6 *addrIn)
            {
                addrIn->sin6_family = AF_INET6;
                addrIn->sin6_port = htons(port);
                if (::inet_pton(AF_INET6, ip.c_str(), &addrIn->sin6_addr) == 0)
                {
                    return true;
                }
                return false;
            }

            int SocketUtility::CreateNonblocking(sa_family_t family)
            {
                int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
                if (sockfd <= 0)
                {
                    int origin_errno = errno;
                    LOG_ERROR("failed to create socket, errno: {0}", origin_errno);
                }
                return sockfd;
            }

            void SocketUtility::Bind(int sockfd, const SocketAddr &addr)
            {
                int ret = ::bind(sockfd, addr.GetSockAddr(), static_cast<socklen_t>(sizeof(sockaddr_in6)));
                if (ret < 0)
                {
                    int origin_errno = errno;
                    LOG_ERROR("failed to bind socket, fd: {0}, errno: {1}", sockfd, origin_errno);
                }
            }

            void SocketUtility::Listen(int sockfd)
            {
                int ret = ::listen(sockfd, SOMAXCONN);
                if (ret < 0)
                {
                    int origin_errno = errno;
                    LOG_ERROR("failed to listen socket, fd: {0}, errno: {1}", sockfd, origin_errno);
                }
            }

            int SocketUtility::Accept(int sockfd, SocketAddr *addr)
            {
                struct sockaddr_in6 sockadd_in;
                socklen_t sockadd_len = static_cast<socklen_t>(sizeof(sockadd_in));
                int connFd = ::accept4(sockfd, static_cast<struct sockaddr *>((void *)&sockadd_in), &sockadd_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
                addr->SetSocketAddrInet(sockadd_in);
                if (connFd < 0)
                {
                    int origin_errno = errno;
                    switch (origin_errno)
                    {
                    case EAGAIN:
                    case ECONNABORTED:
                    case EINTR:
                    case EPROTO:
                    case EPERM:
                    case EMFILE:
                        // expected errors
                        errno = origin_errno;
                        break;
                    case EBADF:
                    case EFAULT:
                    case EINVAL:
                    case ENFILE:
                    case ENOBUFS:
                    case ENOMEM:
                    case ENOTSOCK:
                    case EOPNOTSUPP:
                        // unexpected errors
                        LOG_ERROR("unexpected error on ::accept, fd: {0}, errno: {1}", sockfd, origin_errno);
                        break;
                    default:
                        LOG_ERROR("unknown error on ::accept, fd: {0}, errno: {1}", sockfd, origin_errno);
                        break;
                    }
                }
                return connFd;
            }

            int SocketUtility::Connect(int sockfd, const struct SocketAddr &addr)
            {
                int ret = ::connect(sockfd, static_cast<sockaddr *>((void *)(addr.GetSockAddr())), static_cast<socklen_t>(sizeof(addr)));
                if (ret < 0)
                {
                    int origin_errno = errno;
                    //LOG_ERROR("failed to connect socket, fd: {0}, remote_addr: {1} , errno: {2}", sockfd, addr.ToString(), origin_errno);
                }
                return ret;
            }

            ssize_t SocketUtility::Read(int sockfd, void *buf, size_t buf_size)
            {
                return ::read(sockfd, buf, buf_size);
            }

            ssize_t SocketUtility::ReadV(int sockfd, const iovec *iov, int iov_cnt)
            {
                return ::readv(sockfd, iov, iov_cnt);
            }

            ssize_t SocketUtility::Write(int sockfd, const void *buf, size_t buf_size)
            {
                return ::write(sockfd, buf, buf_size);
            }

            void SocketUtility::Close(int sockfd)
            {
                ::close(sockfd);
            }

            int SocketUtility::SocketError(int sockfd)
            {
                int opt_val;
                socklen_t len = static_cast<socklen_t>(sizeof(opt_val));
                int ret = ::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &opt_val, &len);
                if (ret < 0)
                {
                    opt_val = errno;
                    LOG_ERROR("failed to get socket error, fd: {0}, errno: {1}", sockfd, errno);
                }
                return opt_val;
            }

            sockaddr_in6 SocketUtility::GetPeerAddr(int sockfd)
            {
                struct sockaddr_in6 peeraddr;
                memset(&peeraddr, 0, sizeof(peeraddr));
                socklen_t len = static_cast<socklen_t>(sizeof(peeraddr));
                int ret = ::getpeername(sockfd, static_cast<struct sockaddr *>((void *)(&peeraddr)), &len);
                if (ret < 0)
                {
                    LOG_ERROR("failed to get peer address, fd: {0}, errno: {1}", sockfd, errno);
                }
                return peeraddr;
            }

            sockaddr_in6 SocketUtility::GetLocalAddr(int sockfd)
            {
                struct sockaddr_in6 localaddr;
                memset(&localaddr, 0, sizeof(localaddr));
                socklen_t len = static_cast<socklen_t>(sizeof(localaddr));
                int ret = ::getsockname(sockfd, static_cast<struct sockaddr *>((void *)(&localaddr)), &len);
                if (ret < 0)
                {
                    LOG_ERROR("failed to get local address, fd: {0}, errno: {1}", sockfd, errno);
                }
                return localaddr;
            }

            void SocketUtility::SetTcpNoDelay(int sockfd, bool on)
            {
                int opt_val = on ? 1 : 0;
                int ret = ::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
                if (ret < 0)
                {
                    LOG_ERROR("failed to set TCP_NODELAY option, fd: {0}, errno: {1}", sockfd, errno);
                }
            }

            void SocketUtility::SetKeepAlived(int sockfd, bool on)
            {
                int opt_val = on ? 1 : 0;
                int ret = ::setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
                if (ret < 0)
                {
                    LOG_ERROR("failed to set SO_KEEPALIVE option, fd: {0}, errno: {1}", sockfd, errno);
                }
            }

            void SocketUtility::SetReuseAddr(int sockfd, bool on)
            {
                int opt_val = on ? 1 : 0;
                int ret = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
                if (ret < 0)
                {
                    LOG_ERROR("failed to set SO_REUSEADDR option, fd: {0}, errno: {1}", sockfd, errno);
                }
            }

            void SocketUtility::SetReusePort(int sockfd, bool on)
            {
                int opt_val = on ? 1 : 0;
                int ret = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
                if (ret < 0)
                {
                    LOG_ERROR("failed to set SO_REUSEPORT option, fd: {0}, errno: {1}", sockfd, errno);
                }
            }

            void SocketUtility::ShutdownWrite(int sockfd)
            {
                ::shutdown(sockfd, SHUT_WR);
            }
        }
    }
}
