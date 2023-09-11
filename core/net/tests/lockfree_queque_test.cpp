/*
 * @Author: sunzhenbao && sunzhenbao@live.com
 * @Date: 2023-08-31 16:39:05
 * @LastEditors: sunzhenbao sunzhenbao@cncytech.com
 * @LastEditTime: 2023-08-31 19:29:19
 * @FilePath: /network-poller/core/net/tests/lockfree_queque_test.cpp
 * @Description:
 *
 * Copyright (c) 2023 by sunzhenbao.live.com, All Rights Reserved.
  * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <readerwriterqueue.h>
#include <concurrentqueue.h>
#include <iostream>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <ratio>
#include <unistd.h>
#include <time.h>
#include <atomic>
#define CompilerMemBar() std::atomic_signal_fence(std::memory_order_seq_cst)

typedef timespec SystemTime;
using namespace moodycamel;
using namespace std::chrono;

ConcurrentQueue<int> queue_task_funcs_(1024);
duration<double, std::ratio<1, 1>> read_time;
duration<double, std::ratio<1, 1>> write_time;

void ReadFunc()
{
    uint64_t read_cnt = 0;
    high_resolution_clock::time_point read_end;
    high_resolution_clock::time_point read_start;
    while (1)
    {
        int k;
        if (queue_task_funcs_.try_dequeue(k))
        {
            if (read_cnt == 0)
            {
                read_start = high_resolution_clock::now();
            }
            read_cnt++;
        }
        if (read_cnt > 200000000)
        {
            read_end = high_resolution_clock::now();
            break;
        }
    }
    duration<double, std::ratio<1, 1>> duration_s(read_end - read_start);
    read_time = duration_s;
}

void WriteFunc()
{
    uint64_t write_cnt = 0;

    high_resolution_clock::time_point write_end;
    high_resolution_clock::time_point write_start;

    while (1)
    {
        // usleep(1);
        int k;
        if (queue_task_funcs_.enqueue(k))
        {
            write_cnt++;
            if (write_cnt == 1)
            {
                write_start = high_resolution_clock::now();
            }
        }
        if (write_cnt > 200000000)
        {
            write_end = high_resolution_clock::now();
            break;
        }
    }
    duration<double, std::ratio<1, 1>> duration_s(write_end - write_start);
    write_time = duration_s;
}

SystemTime getSystemTime()
{
    timespec t;
    CompilerMemBar();
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &t) != 0)
    {
        t.tv_sec = (time_t)-1;
        t.tv_nsec = -1;
    }
    CompilerMemBar();

    return t;
}

double getTimeDelta(SystemTime start)
{
    timespec t;
    CompilerMemBar();
    if ((start.tv_sec == (time_t)-1 && start.tv_nsec == -1) || clock_gettime(CLOCK_MONOTONIC_RAW, &t) != 0)
    {
        return -1;
    }
    CompilerMemBar();

    return static_cast<double>(static_cast<long>(t.tv_sec) - static_cast<long>(start.tv_sec)) * 1000 + double(t.tv_nsec - start.tv_nsec) / 1000000;
}

int main(int argc, char **args)
{
    int item = 1;
    int item_rec;
    auto start = getSystemTime();
    for (int i = 0; i != 200000000; ++i)
    {
        queue_task_funcs_.enqueue(item);
        //queue_task_funcs_.try_dequeue(item_rec);
    }
    double test =  getTimeDelta(start);

    std::thread read(&ReadFunc);
    std::thread write(&WriteFunc);
    read.join();
    write.join();

    std::cout << "write 200000000, time: " << write_time.count() << " seconds" << std::endl;
    std::cout << "read 200000000, time: " << read_time.count() << " seconds" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10));
}