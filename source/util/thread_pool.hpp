#pragma once
#include <cstdint>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>
#include <future>
#include <condition_variable>
#include <type_traits>
#include <vector>
#include <queue>

// Move-only callable
class Task
{
    using CallablePtr = std::unique_ptr<void, void (*)(void*)>;
    using InvokeFn = void(void*);

public:
    template <typename F>
    Task(F&& f)
        : m_callable(new F(std::forward<F>(f)), [](void* p)
              { delete static_cast<F*>(p); })
        , m_invoke([](void* ptr)
              { (*static_cast<F*>(ptr))(); })
    {
    }

    void operator()() const
    {
        m_invoke(m_callable.get());
    }

    Task(Task&&) = default;
    Task& operator=(Task&&) = default;

    Task() = delete;
    ~Task() = default;

private:
    CallablePtr m_callable;
    InvokeFn* m_invoke = nullptr;
};

class ThreadPool
{
public:
    ThreadPool(uint32_t threads);
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;

    template <typename F, typename... Args>
    decltype(auto) queue_task(F&& callable, Args&&... args);

    auto get_num_threads_working() const { return m_threads_working.load(); }
    auto get_num_threads() const { return m_workers.size(); }

private:
    void worker_loop();
    bool m_exit_code = false;

    std::mutex m_queue_mutex;
    std::condition_variable m_worker_signal;

    std::vector<std::thread> m_workers;
    std::queue<Task> m_tasks;

    std::atomic_size_t m_threads_working { 0 };
};

template <typename F, typename... Args>
decltype(auto) ThreadPool::queue_task(F&& callable, Args&&... args)
{
    using ReturnType = std::invoke_result_t<F, Args...>;

    std::packaged_task<ReturnType()> task(std::bind(std::forward<F>(callable), std::forward<Args>(args)...));
    auto task_future = task.get_future();

    {
        std::unique_lock<std::mutex> const lock(m_queue_mutex);
        m_tasks.emplace(std::move(task));
    }

    m_worker_signal.notify_one();
    return task_future;
}

inline ThreadPool threads { std::thread::hardware_concurrency() };