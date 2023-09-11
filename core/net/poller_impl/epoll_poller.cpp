#include "epoll_poller.h"

#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include "net/event/event_channel.h"

namespace liang
{
    namespace net
    {
        EpollPoller::EpollPoller(EventLoop *loop)
            : Poller(loop),
              epoll_fd_(::epoll_create1(EPOLL_CLOEXEC)),
              events_ready(16)
        {
        }

        EpollPoller::~EpollPoller()
        {
            ::close(epoll_fd_);
        }

        void EpollPoller::Poll(int timeoutMS, EventConextList *active_evt_channel)
        {
            Poller::Poll(timeoutMS, active_evt_channel);

            int evt_number = ::epoll_wait(epoll_fd_, &*events_ready.begin(), static_cast<int>(events_ready.size()), timeoutMS);
            if (evt_number > 0)
            {
                // events happened
                for (int i = 0; i < evt_number; i++)
                {
                    // get the context of the event
                    EventChannel *evt_channel = static_cast<EventChannel *>(events_ready[i].data.ptr);
                    // event context's file descriptor
                    int fd = evt_channel->Fd();
                    // save the events happened on the fd of the event context
                    evt_channel->SetRecievedEvent(events_ready[i].events);
                    // add event context to active list to wait for loop
                    active_evt_channel->push_back(evt_channel);
                }
                if (evt_number == events_ready.size())
                {
                    events_ready.resize(events_ready.size() * 2);
                }
            }
            else if (evt_number == 0)
            {
                // nothing event
            }
            else
            {
                if (errno != EINTR)
                {
                    // error happened
                }
            }
        }

        void EpollPoller::UpdateEventChannel(EventChannel *evt_channel)
        {
            Poller::UpdateEventChannel(evt_channel);
            int fd = evt_channel->Fd();
            switch (evt_channel->StatusInPoller())
            {
            case ChannelStatus::NEW_CHANNEL: // never added into event context list
                assert(evt_channel_list_.find(fd) == evt_channel_list_.end());
                evt_channel_list_[fd] = evt_channel;
                evt_channel->SetStatusInPoller(ChannelStatus::ADDED_CHANNEL); // update status to added
                UpdateEpollCtrl(EPOLL_CTL_ADD, evt_channel);                  // epoll ctrl add
                break;
            case ChannelStatus::ADDED_CHANNEL:
                assert(evt_channel_list_.find(fd) != evt_channel_list_.end());
                assert(evt_channel_list_[fd] == evt_channel);
                if (evt_channel->IsNoneEvent()) // none event
                {
                    UpdateEpollCtrl(EPOLL_CTL_DEL, evt_channel);                   // so delete from epoll.
                    evt_channel->SetStatusInPoller(ChannelStatus::DELETE_CHANNEL); //
                }
                else
                {
                    UpdateEpollCtrl(EPOLL_CTL_MOD, evt_channel); // epoll modify
                }
                break;
            case ChannelStatus::DELETE_CHANNEL: // added in list ago, but none events.
                assert(evt_channel_list_.find(fd) != evt_channel_list_.end());
                evt_channel_list_[fd] = evt_channel;
                evt_channel->SetStatusInPoller(ChannelStatus::ADDED_CHANNEL); // update status to added
                UpdateEpollCtrl(EPOLL_CTL_ADD, evt_channel);                  // re-add epoll ctrl
                break;
            default:
                break;
            }
        }

        void EpollPoller::RemoveEventChannel(EventChannel *evt_channel)
        {
            Poller::RemoveEventChannel(evt_channel);
            int fd = evt_channel->Fd();
            assert(evt_channel_list_[fd] == evt_channel); // existed
            assert(evt_channel->IsNoneEvent());       // should be none event before it's erased from list.
            auto status = evt_channel->StatusInPoller();
            assert(status == ChannelStatus::ADDED_CHANNEL || status == ChannelStatus::DELETE_CHANNEL);
            evt_channel_list_.erase(fd);
            if (evt_channel->StatusInPoller() == ChannelStatus::ADDED_CHANNEL)
            {
                UpdateEpollCtrl(EPOLL_CTL_DEL, evt_channel);
            }
            evt_channel->SetStatusInPoller(ChannelStatus::NEW_CHANNEL);
        }

        void EpollPoller::UpdateEpollCtrl(int operation, EventChannel *evt_channel)
        {
            struct epoll_event evt;
            memset(&evt, 0, sizeof(evt));
            evt.events = evt_channel->Events();
            evt.data.ptr = evt_channel;
            int fd = evt_channel->Fd();
            int ret = ::epoll_ctl(epoll_fd_, operation, fd, &evt);
            if (ret < 0)
            {
                // log
            }
        }
    }
}
