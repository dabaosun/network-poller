/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-19 21:19:49
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 17:28:25
 * @FilePath: /network-poller/core/net/event/event_channel.h
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#pragma once

#include <functional>
#include <memory>

#include "base/noncopyable.h"
#include "net/event/event_loop.h"

namespace liang
{
    namespace net
    {
        typedef std::function<void()> ReadEventCB;  // fd read call back
        typedef std::function<void()> WriteEventCB; // fd write call back
        typedef std::function<void()> CloseEventCB; // fd write call back
        typedef std::function<void()> ErrorEventCB; // fd error call back

        enum ChannelStatus
        {
            NEW_CHANNEL,
            ADDED_CHANNEL,
            DELETE_CHANNEL,
        };

        class EventChannel : base::noncopyable
        {
        public:
            EventChannel(EventLoop *loop, int fd);

            void SetReadEventCallBack(ReadEventCB callback);   // set channel read event
            void SetWriteEventCallBack(WriteEventCB callback); // set channel read event
            void SetErrorEventCallBack(CloseEventCB callback);
            void SetCloseEventCallBack(ErrorEventCB callback);

            void HandleEvent(); // handle ready event notified by Poller

        public:
            int Fd() const
            {
                return fd_;
            }

            int Events() const
            {
                return events_;
            }

            EventLoop *OwnerLoop() const
            {
                return event_loop_;
            }

            bool IsNoneEvent() const;
            bool IsReading() const; // read event registed on fd
            bool IsWriting() const; // write event registed on fd

            void EnableReading();  // add read event to fd
            void DisableReading(); // remove read event from fd
            void EnableWriting();  // add write event to fd
            void DisableWriting(); // remove write event to fd
            void DisableAll();     // reset to none event

            void RemoveFromLoop();

            void SetRecievedEvent(int event)
            {
                recieved_events = event;
            }

            ChannelStatus StatusInPoller() const
            {
                return poller_status_;
            }

            void SetStatusInPoller(const ChannelStatus status)
            {
                poller_status_ = status;
            }

            std::string ReceivedEventToString() const;

        private:
            static std::string EventsToString(int fd, int ev);

        private:
            int fd_;             // IO's file descriptor.
            int events_;         // events registed by the fd.
            int recieved_events; // recieved events from epoll or poll
            ChannelStatus poller_status_;

            EventLoop *event_loop_;

            ReadEventCB read_event_cb_;
            WriteEventCB write_event_cb_;
            CloseEventCB close_event_cb_;
            ErrorEventCB error_event_cb_;
        };

    }
}