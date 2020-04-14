#include <gmock/gmock.h>
#include "gtest/gtest.h"

#include "Scheduler.hpp"
#include "ThreadPool.hpp"

TEST(ThreadPool, postJob)
{
    ThreadPool pool(2);

    auto ret = pool.enqueue([](int) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return 42;
    });


    EXPECT_EQ(42, ret.get());
}
