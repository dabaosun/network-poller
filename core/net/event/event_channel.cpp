/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-17 20:27:53
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-28 17:28:48
 * @FilePath: /network-poller/core/net/event/event_channel.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "event_channel.h"
#include <poll.h>
#include <sstream>
#include "net/event/event.h"
#include "base/log/logger.h"

namespace liang
{
    namespace net
    {
        EventChannel::EventChannel(EventLoop *loop, int fd)
            : fd_(fd),
              events_(Event::none_event_flag),
              poller_status_(NEW_CHANNEL),
              event_loop_(loop)
        {
        }

        void EventChannel::SetReadEventCallBack(ReadEventCB callback)
        {
            read_event_cb_ = std::move(callback);
        }

        void EventChannel::SetWriteEventCallBack(WriteEventCB callback)
        {
            write_event_cb_ = std::move(callback);
        }

        void EventChannel::SetErrorEventCallBack(CloseEventCB callback)
        {
            error_event_cb_ = std::move(callback);
        }

        void EventChannel::SetCloseEventCallBack(ErrorEventCB callback)
        {
            close_event_cb_ = std::move(callback);
        }

        void EventChannel::HandleEvent()
        {
            LOG_TRACE("recieved_events: {0}", ReceivedEventToString());

            if ((recieved_events & POLLHUP) && !(POLLIN & recieved_events))
            {
                // closed
                if (close_event_cb_)
                {
                    // call cb.
                    close_event_cb_();
                }
            }

            if (recieved_events & POLLNVAL)
            {
                // invalid
            }

            if (recieved_events & (POLLERR | POLLNVAL))
            {
                // error happened
                if (error_event_cb_)
                {
                    // call cb.
                    error_event_cb_();
                }
            }

            if (recieved_events & (POLLIN | POLLPRI | POLLRDHUP))
            {
                // read event
                if (read_event_cb_)
                {
                    // call cb.
                    read_event_cb_();
                }
            }

            if (recieved_events & POLLOUT)
            {
                // write event
                if (write_event_cb_)
                {
                    // call cb.
                    write_event_cb_();
                }
            }
        }

        bool EventChannel::IsNoneEvent() const
        {
            return Event::none_event_flag == events_;
        }

        bool EventChannel::IsReading() const
        {
            return Event::read_event_flag & events_;
        }

        bool EventChannel::IsWriting() const
        {
            return Event::write_event_flag & events_;
        }

        void EventChannel::EnableReading()
        {
            // set read event flag
            events_ |= Event::read_event_flag;
            // notify the loop to update the context
            event_loop_->UpdateEventChannel(this);
        }

        void EventChannel::DisableReading()
        {
            events_ &= ~Event::read_event_flag;
            // notify the loop to update the context
            event_loop_->UpdateEventChannel(this);
        }

        void EventChannel::EnableWriting()
        {
            events_ |= Event::write_event_flag;
            // notify the loop to update the context
            event_loop_->UpdateEventChannel(this);
        }

        void EventChannel::DisableWriting()
        {
            events_ &= ~Event::write_event_flag;
            // notify the loop to update the context
            event_loop_->UpdateEventChannel(this);
        }

        void EventChannel::DisableAll()
        {
            events_ = Event::none_event_flag;
            // notify the loop to update the context
            event_loop_->UpdateEventChannel(this);
        }

        void EventChannel::RemoveFromLoop()
        {
            event_loop_->RemoveEventChannel(this);
        }

        std::string EventChannel::ReceivedEventToString() const
        {
            return EventChannel::EventsToString(this->fd_, this->events_);
        }

        std::string EventChannel::EventsToString(int fd, int ev)
        {
            std::ostringstream oss;
            oss << fd << ": ";
            if (ev & POLLIN)
                oss << "IN ";
            if (ev & POLLPRI)
                oss << "PRI ";
            if (ev & POLLOUT)
                oss << "OUT ";
            if (ev & POLLHUP)
                oss << "HUP ";
            if (ev & POLLRDHUP)
                oss << "RDHUP ";
            if (ev & POLLERR)
                oss << "ERR ";
            if (ev & POLLNVAL)
                oss << "NVAL ";

            return oss.str();
        }
    }
}
