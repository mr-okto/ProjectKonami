//
// Created by user on 11.04.2020.
//

#include "Scheduler.hpp"

void Scheduler::add_task(Clock::time_point time, std::shared_ptr<Task> t) {
    std::lock_guard<std::mutex> l(lock_);
    tasks_.emplace(time, std::move(t));
    sleeper_.interrupt();
}
