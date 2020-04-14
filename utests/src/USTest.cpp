#include "UserService.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockSessionManager {
 public:
    MOCK_METHOD0(get_online_users, std::vector<unsigned int>());
};

TEST(USGetOnlineUsers, ok) {
    MockSessionManager object;
    EXPECT_CALL(object, get_online_users()).Times(testing::AtLeast(1));

    chat::UserService u_s;
    u_s.get_online_users(4);
}