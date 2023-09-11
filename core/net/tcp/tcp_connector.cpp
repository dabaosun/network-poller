#include "tcp_connector.h"
#include <assert.h>
#include "net/socket/socket_utility.h"
#include "base/log/logger.h"

using namespace liang::net::socket;

namespace liang
{
    namespace net
    {
        namespace tcp
        {
            void TcpConnector::Start()
            {
                States expected = kDisconnected;
                if (this->state_.compare_exchange_weak(expected, kConnecting, std::memory_order_relaxed))
                {
                    loop_->RunInLoop(std::bind(&TcpConnector::StartInLoop, this));
                    return;
                }

                LOG_LOG("started ago, nothing to do");
            }

            void TcpConnector::Stop()
            {
                if (kDisconnected == state_)
                {
                    LOG_LOG("stopped ago, nothing to do");
                    return;
                }
                loop_->RunInLoop(std::bind(&TcpConnector::StopInLoop, this));
            }

            void TcpConnector::StartInLoop()
            {
                assert(loop_->IsInLoopThread());
                assert(kConnecting == state_);

                int fd = SocketUtility::CreateNonblocking(addr_.Family());
                int ret = SocketUtility::Connect(fd, addr_);
                int orig_errno = errno;
                switch (orig_errno)
                {
                case 0:
                case EINPROGRESS:
                case EINTR:
                case EISCONN: // connected/connecting
                    assert(loop_->IsInLoopThread());
                    this->channel_.reset(new EventChannel(loop_, fd));
                    channel_->SetWriteEventCallBack(std::bind(&TcpConnector::WriteEventHandler, this));
                    channel_->SetErrorEventCallBack(std::bind(&TcpConnector::ErrorEventHandler, this));
                    channel_->EnableWriting(); // enable epoll ctl.
                    break;
                case EAGAIN:
                case EADDRINUSE:
                case EADDRNOTAVAIL:
                case ECONNREFUSED:
                case ENETUNREACH: // temporaliy error,retry
                    LOG_ERROR("tcp client connect failed, fd: {0}, remote_addr: {1}, errno: {2}", fd, addr_.ToString(), orig_errno);
                    SocketUtility::Close(fd);
                    break;
                case EACCES:
                case EPERM:
                case EAFNOSUPPORT:
                case EALREADY:
                case EBADF:
                case EFAULT:
                case ENOTSOCK: // error, discard to connect
                    LOG_ERROR("tcp client connect failed, fd: {0}, remote_addr: {1}, errno: {2}", fd, addr_.ToString(), orig_errno);
                    SocketUtility::Close(fd);
                    break;
                default:
                    LOG_ERROR("tcp client connect failed, fd: {0}, remote_addr: {1}, errno: {2}", fd, addr_.ToString(), orig_errno);
                    SocketUtility::Close(fd);
                    break;
                }
            }

            void TcpConnector::StopInLoop()
            {
            }

            void TcpConnector::RetryConn(int fd)
            {
                SocketUtility::Close(fd);
                loop_->QueueInLoop(std::bind(&TcpConnector::StartInLoop, this));
            }

            void TcpConnector::WriteEventHandler()
            {
                // connection established
                assert(kConnecting == state_);
                int fd = RemoveChannel();
                int err = SocketUtility::SocketError(fd);
                if (err)
                {
                    // TODO
                    LOG_ERROR("WriteEventHandler, fd:{0}, errno: {1}", fd, errno);
                    RetryConn(fd);
                }
                else
                {
                    if (new_connection_cb_)
                    {
                        new_connection_cb_(channel_->Fd());
                    }
                }
            }

            void TcpConnector::ErrorEventHandler()
            {
            }

            void TcpConnector::ResetChannelPtr()
            {
                channel_.reset();
            }

            int TcpConnector::RemoveChannel()
            {
                channel_->DisableAll();
                channel_->RemoveFromLoop(); // because connection's channel will be trigger into loop in future.
                int fd = channel_->Fd();
                loop_->QueueInLoop(std::bind(&TcpConnector::ResetChannelPtr, this));
                return fd;
            }
        }
    }
}
