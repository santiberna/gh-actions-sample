#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include <functional>
#include <future>
#include <condition_variable>
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
        : callable(new F(std::forward<F>(f)), [](void* p)
              { delete static_cast<F*>(p); })
        , invoke([](void* ptr)
              { (*static_cast<F*>(ptr))(); })
    {
    }

    void operator()() const
    {
        invoke(callable.get());
    }

    Task(Task&&) = default;
    Task& operator=(Task&&) = default;

    Task() = delete;
    ~Task() = default;

private:
    CallablePtr callable;
    InvokeFn* invoke = nullptr;
};

class ThreadPool
{
public:
    ThreadPool(uint32_t threads);
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;

    template <typename F, typename... Args>
    decltype(auto) QueueTask(F&& callable, Args&&... args);

    auto GetNumThreadsWorking() const { return threads_working.load(); }
    auto GetNumThreads() const { return workers.size(); }

private:
    void worker_loop();
    bool exit_code = false;

    std::mutex queue_mutex;
    std::condition_variable worker_signal;

    std::vector<std::thread> workers;
    std::queue<Task> tasks;

    std::atomic_size_t threads_working { 0 };
};

template <typename F, typename... Args>
decltype(auto) ThreadPool::QueueTask(F&& callable, Args&&... args)
{
    using ReturnType = std::invoke_result_t<F, Args...>;

    std::packaged_task<ReturnType()> task(std::bind(std::forward<F>(callable), std::forward<Args>(args)...));
    auto taskFuture = task.get_future();

    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(std::move(task));
    }

    worker_signal.notify_one();
    return taskFuture;
}

inline ThreadPool threads { std::thread::hardware_concurrency() };