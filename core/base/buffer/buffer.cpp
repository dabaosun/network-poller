/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-23 16:15:49
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-09-11 12:51:19
 * @FilePath: /network-poller/core/base/buffer/buffer.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause 
 */

#include "buffer.h"
#include <errno.h>
#include <sys/uio.h>
#include "net/socket/socket_utility.h"

namespace liang
{
    namespace base
    {
        const uint32_t Buffer::InitSize = 1024;

        Buffer::Buffer(uint32_t size)
            : read_index_(0),
              write_index_(0),
              buffer_(size)
        {
        }
        Buffer::~Buffer()
        {
        }

        void Buffer::Append(const char *data, size_t len)
        {
            // ensure size enough before append
            EnsureWritableBytes(len);

            std::copy(data, data + len, buffer_.begin() + write_index_);
            write_index_ += len;
        }

        void Buffer::Append(const std::string &str)
        {
            const char *data = &(*str.begin());
            Append(data, str.size());
        }

        void Buffer::EnsureWritableBytes(size_t len)
        {
            if (WritableBytes() < len)
            {
                // resize enough
                buffer_.resize(write_index_ + len);
            }
        }

        uint32_t Buffer::WritableBytes() const
        {
            // total - used
            return buffer_.size() - write_index_;
        }

        uint32_t Buffer::ReadableBytes() const
        {
            // written - read
            return write_index_ - read_index_;
        }

        int Buffer::ReadFromFD(int fd, int &errorNo)
        {
            // saved an ioctl()/FIONREAD call to tell how much to read
            char extrabuf[65536];
            struct iovec vec[2];
            uint32_t writable = WritableBytes();
            vec[0].iov_base = &(*buffer_.begin()) + write_index_;
            vec[0].iov_len = writable;
            vec[1].iov_base = extrabuf;
            vec[1].iov_len = sizeof extrabuf;

            uint32_t iovcnt = (writable < sizeof extrabuf) ? 2 : 1;            // when there is enough space in this buffer, don't read into extrabuf.
            int n = liang::net::socket::SocketUtility::ReadV(fd, vec, iovcnt); // when extrabuf is used, we read 128k-1 bytes at most.
            if (n < 0)
            {
                // error happened
                errorNo = errno;
            }
            else if ((uint32_t)n <= writable)
            {
                write_index_ += n; // only add write index
            }
            else
            {
                write_index_ = buffer_.size();
                Append(extrabuf, n - writable); // resize buffer and append extrabuf used to it.
            }
            return n;
        }

        void Buffer::Swap(Buffer &rhs)
        {
            buffer_.swap(rhs.buffer_);
            std::swap(read_index_, rhs.read_index_);
            std::swap(write_index_, rhs.write_index_);
        }

    }
}
