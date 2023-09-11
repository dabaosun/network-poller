#include "event_loop_thread.h"
#include "net/event/event_loop.h"
#include "base/log/logger.h"

extern std::ostream &operator<<(std::ostream &os, const std::thread::id &thread_id);

namespace liang
{
    namespace net
    {
        EventLoopThread::EventLoopThread(const std::string &name) : EncThread(name),
                                                                    loop_(nullptr)
        {
        }

        EventLoopThread::~EventLoopThread()
        {
            LOG_DEBUG("~EventLoopThread");
            if (nullptr != loop_) //the thread is running loop
            {
                LOG_DEBUG("call loop_->Quit()");
                loop_->Quit(); //notifiy the loop exit.
            }
        }

        EventLoop *EventLoopThread::GetLoopInThread()
        {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (nullptr == loop_)
                {
                    cond_.wait(lock);
                }
            }
            return loop_;
        }

        void EventLoopThread::run()
        {
            EventLoop loop(name_);
            {
                std::unique_lock<std::mutex> lock(mutex_);
                this->loop_ = &loop;
                cond_.notify_one();
            }
            loop.Loop();
        }
    }
}