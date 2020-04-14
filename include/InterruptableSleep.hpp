#ifndef MESSENGER_PROJECT_INTERRUPTABLESLEEP_HPP
#define MESSENGER_PROJECT_INTERRUPTABLESLEEP_HPP

#include <chrono>
#include <mutex>
#include <future>

using Clock = std::chrono::system_clock;

class InterruptableSleep {
private:
    bool is_interrupted_;
    std::mutex m;
    std::condition_variable cv;

public:
    InterruptableSleep() : is_interrupted_(false) {};
    InterruptableSleep(const InterruptableSleep&) = delete;
    InterruptableSleep(InterruptableSleep&&) = delete;
    ~InterruptableSleep() = default;

    InterruptableSleep&operator=(const InterruptableSleep&) = delete;
    InterruptableSleep&operator=(InterruptableSleep&&) = delete;

    void sleep() {
        std::unique_lock<std::mutex> ul(m);
        cv.wait(ul, [this] { return is_interrupted_; });
        is_interrupted_ = false;
    }

    void sleep_for(Clock::time_point time) {
        std::unique_lock<std::mutex> ul(m);
        cv.wait_until(ul, time, [this] { return is_interrupted_; });
        is_interrupted_ = false;
    }

    void interrupt() {
        std::lock_guard<std::mutex> lg(m);
        is_interrupted_ = true;
        cv.notify_one();
    }

};


#endif //MESSENGER_PROJECT_INTERRUPTABLESLEEP_HPP
