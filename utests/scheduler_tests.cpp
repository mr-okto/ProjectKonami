#include <gmock/gmock.h>
#include "gtest/gtest.h"

#include "Scheduler.hpp"

const float time_err = 0.001F;

// that's class-helper for tests that serializes working logic of scheduler
class TimeCounter {
public:

    TimeCounter() : scheduler(2), time(std::chrono::system_clock::now()), done(false) {};

    void in(std::chrono::system_clock::duration dur) {
//        scheduler.in(dur, [this] {
//            time = std::chrono::system_clock::now();
//            done = true;
//        });
        return;
    };

    float get_remain_seconds_of_first_task() const {
        using second_t = std::chrono::duration<float, std::ratio<1> >;

        auto a = scheduler.time_point_of_first_task();
        auto b = std::chrono::system_clock::now();

        auto c = std::chrono::duration_cast<second_t >(a - b).count();
        return c;
    };

    std::chrono::system_clock::time_point current_time() const { return time;};
    bool is_done() const { return done; };

private:
    Scheduler scheduler;
    std::chrono::system_clock::time_point time;
    bool done;
};

TEST(SchedulerTest, CanScheduleTest) {
    using clock_t = std::chrono::system_clock;
    using second_t = std::chrono::duration<float, std::ratio<1> >;

    TimeCounter timeCounter;

    clock_t::time_point start = clock_t::now();
    timeCounter.in(std::chrono::seconds(2));

//    while (!timeCounter.is_done()) {};

    clock_t::time_point end = clock_t::now();
    std::cout << "DIFFERENCE END - START " << std::chrono::duration_cast<second_t>(end - start).count() << std::endl;

    EXPECT_NEAR(2.F, std::chrono::duration_cast<second_t>(end - start).count(), time_err);
}

TEST (SchedulerTest, CorrectRemainTimeTest) {

    TimeCounter timeCounter;

    timeCounter.in(std::chrono::seconds(5));
    sleep(1);

    EXPECT_NEAR(4.F, timeCounter.get_remain_seconds_of_first_task(), time_err);
}