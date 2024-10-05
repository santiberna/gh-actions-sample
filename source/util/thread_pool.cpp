#include "thread_pool.hpp"
#include <cstdint>
#include <mutex>
#include <utility>

ThreadPool::ThreadPool(uint32_t thread_count)
{
    for (uint32_t i = 0; i < thread_count; ++i)
    {
        workers.emplace_back(&ThreadPool::worker_loop, this);
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> const lock(queue_mutex);
        exit_code = true;
    }
    worker_signal.notify_all();

    for (std::thread& active_thread : workers)
    {
        active_thread.join();
    }

    workers.clear();
}

void ThreadPool::worker_loop()
{
    while (true)
    {

        std::unique_lock<std::mutex> lock(queue_mutex);
        worker_signal.wait(lock, [this]
            { return !tasks.empty() || exit_code; });

        if (exit_code)
        {
            return;
        }

        auto task = std::move(tasks.front());
        tasks.pop();

        lock.unlock();

        threads_working.fetch_add(1);
        task();
        threads_working.fetch_sub(1);
    }
}