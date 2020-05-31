#ifndef MESSENGER_PROJECT_SCHEDULER_HPP
#define MESSENGER_PROJECT_SCHEDULER_HPP

#include <iomanip>
#include <chrono>
#include <functional>
#include <atomic>
#include <map>

#include "InterruptableSleep.hpp"
#include "ThreadPool.hpp"

using Clock = std::chrono::system_clock;

class Task {
public:
    explicit Task(std::function<void()> && f, bool repeat = false) :
        task_(std::move(f)),
        repeatable_(repeat) {};

    virtual Clock::time_point get_next_new_time() const = 0;

    std::function<void()> task_;
    bool repeatable_;

};

class InTask : public Task {
public:
    explicit InTask(std::function<void()> && f) : Task(std::move(f)) {};

    // dummy time_point because it's not used
    [[nodiscard]] Clock::time_point get_next_new_time() const override { return Clock::time_point(Clock::duration(0)); };
};

class EveryTask : public Task {
public:
    explicit EveryTask(Clock::duration time, std::function<void()> && f) :
        Task(std::move(f), true),
        time_(time) {};

    [[nodiscard]] Clock::time_point get_next_new_time() const override { return Clock::now() + time_; };

    Clock::duration time_;
};

class Scheduler {
private:
    std::atomic<bool> is_done_;

    InterruptableSleep sleeper_;
    std::multimap<Clock::time_point, std::shared_ptr<Task>> tasks_;
    std::mutex lock_;

    ThreadPool threads_;

    void add_task(Clock::time_point time, std::shared_ptr<Task> t);

    void manage_tasks();

public:
    explicit Scheduler(unsigned int max_n_tasks = 2) :
        is_done_(false),
        threads_(max_n_tasks + 1) {};

    Scheduler(const Scheduler &) = delete;

    Scheduler(Scheduler &&) noexcept = delete;

    Scheduler &operator=(const Scheduler &) = delete;

    Scheduler &operator=(Scheduler &&) noexcept = delete;

    ~Scheduler() {
        is_done_ = true;
        sleeper_.interrupt();
    }

    Clock::time_point time_point_of_first_task() const { return tasks_.begin()->first;};

    template<typename _Callable, typename... _Args>
    void in(const Clock::time_point time, _Callable &&f, _Args &&... args) {
        std::shared_ptr<Task> t = std::make_shared<InTask>(
                std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...));
        add_task(time, std::move(t));
    }

    template<typename _Callable, typename... _Args>
    void in(const Clock::duration time, _Callable &&f, _Args &&... args) {
        in(Clock::now() + time, std::forward<_Callable>(f), std::forward<_Args>(args)...);
    }

    template<typename _Callable, typename... _Args>
    void every(const Clock::duration time, _Callable &&f, _Args &&... args) {
        std::shared_ptr<Task> t = std::make_shared<EveryTask>(time, std::bind(std::forward<_Callable>(f),
                                                                              std::forward<_Args>(args)...));
        auto next_time = t->get_next_new_time();
        add_task(next_time, std::move(t));
    }

};


#endif //MESSENGER_PROJECT_SCHEDULER_HPP
