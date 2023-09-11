/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-23 15:11:48
 * @LastEditors: sunzhenbao sunzhenbao@live.com
 * @LastEditTime: 2023-08-23 16:13:59
 * @FilePath: /network-poller/core/base/buffer/buffer.h
 * @Description: 
 * 
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#pragma once

#include <vector>
#include <stdint.h>
#include <iostream>
#include <algorithm>

namespace liang
{
    namespace base
    {
        class Buffer
        {
        public:
            static const uint32_t InitSize;

            Buffer(uint32_t size = InitSize);
            ~Buffer();

            int ReadFromFD(int fd, int &errorNo);

            uint32_t WritableBytes() const; 
            uint32_t ReadableBytes() const;

            bool IsReadble()
            {
                return ReadableBytes() > 0 ? true : false;
            }

            bool IsEmpty()
            {
                return !IsReadble();
            }

            void Append(const char *data, std::size_t len);
            void Append(const std::string &str);

            void EnsureWritableBytes(std::size_t len);

            const uint8_t *ReadIndexPtr() const
            {
                return BufferPtr() + read_index_;
            }

            void ResetIndex()
            {
                read_index_ = 0;
                write_index_ = 0;
            }

            void ClearReadIndex(uint32_t len)
            {
                if (len < ReadableBytes())
                {
                    read_index_ += len;
                }
                else
                {
                    ResetIndex();
                }
            }

            int ReadAllAsString(std::string &readBuf)
            {
                return ReadAsString(ReadableBytes(), readBuf);
            }

            int ReadAsString(uint32_t len, std::string &readBuf)
            {
                if (len > ReadableBytes())
                    len = ReadableBytes();
                readBuf.assign((const char *)ReadIndexPtr(), len);
                ClearReadIndex(len);
                return len;
            }

            int ReadAllAsByte(std::vector<uint8_t> &data)
            {
                return ReadBytes(data, ReadableBytes());
            }

            int ReadBytes(std::vector<uint8_t> &data, uint32_t len)
            {
                if (len > ReadableBytes())
                    len = ReadableBytes();
                data.assign(ReadIndexPtr(), ReadIndexPtr() + len);
                ClearReadIndex(len);
                return len;
            }

            void Swap(Buffer &rhs);

            const char *FindCRLF() const
            {
                char kCRLF[] = "\r\n";
                // const char* writePtr = BufferPtr()+write_index_;
                const char *crlf = std::search((char *)ReadIndexPtr(), (char *)BufferPtr() + write_index_, kCRLF, kCRLF + 2);
                return crlf == ((char *)BufferPtr() + write_index_) ? nullptr : crlf;
            }
            void RetrieveUntil(const char *end)
            {
                if (!(ReadIndexPtr() <= (uint8_t *)end))
                {
                    std::cout << "error RetrieveUntil" << std::endl;
                    return;
                }
                if (!(end <= ((char *)BufferPtr() + write_index_)))
                {
                    std::cout << "error RetrieveUntil" << std::endl;
                    return;
                }
                ClearReadIndex((uint8_t *)end - ReadIndexPtr());
            }

        private:
            uint32_t read_index_;
            uint32_t write_index_;

            std::vector<uint8_t> buffer_;

            const uint8_t *BufferPtr() const
            {
                return &(*buffer_.begin());
            }
        };
    }
}
