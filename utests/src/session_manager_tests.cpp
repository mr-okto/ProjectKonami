#include <iostream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "SessionManager.hpp"
#include "Session.hpp"

using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::InSequence;

class SessionManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::unique_ptr default_session = std::make_unique<Session>(Session(0, "default"));
        sessionManager_.add_session(std::move(default_session));
    }

    void TearDown() override {

    }

    SessionManager sessionManager_;
};


TEST_F(SessionManagerTest, AddSessionTest) {
    std::unique_ptr<Session> session = std::make_unique<Session>(Session(1, "token"));

    EXPECT_TRUE(sessionManager_.add_session(std::move(session)));
    EXPECT_FALSE(sessionManager_.add_session(std::move(session))); // сессии уникальны, нельзя 2 раза добавить одну и ту же сессию
}

TEST_F(SessionManagerTest, CloseSessionTest) {
    std::unique_ptr<Session> session = std::make_unique<Session>(Session(1, "token"));

    EXPECT_TRUE(sessionManager_.close_session("token"));
    EXPECT_FALSE(sessionManager_.close_session(1));
}

TEST_F(SessionManagerTest, ValidateSessionTest) {
    Session session(1, "token");

    EXPECT_FALSE(sessionManager_.validate_session(session.token()));
    EXPECT_TRUE(sessionManager_.validate_session(Session(0, "default").token()));
}

TEST_F(SessionManagerTest, GetOnlineUsersTest) {
    std::unique_ptr<Session> session1 = std::make_unique<Session>(Session(1, "token1"));
    std::unique_ptr<Session> session2 = std::make_unique<Session>(Session(2, "token2"));
    std::unique_ptr<Session> session3 = std::make_unique<Session>(Session(3, "token3"));

    EXPECT_TRUE(sessionManager_.add_session(std::move(session1)));
    EXPECT_TRUE(sessionManager_.add_session(std::move(session2)));
    EXPECT_TRUE(sessionManager_.add_session(std::move(session3)));

    std::vector<uint32_t> online_users = sessionManager_.get_online_users();
    EXPECT_EQ(3, online_users.size());

    for (auto user_id : online_users) {
        EXPECT_TRUE(user_id == 1 || user_id == 2 || user_id == 3);
    }
}

TEST_F(SessionManagerTest, GetIdByTokenTest) {
    uint32_t id = 123;
    std::string token = "asdfghjkl";

    sessionManager_.add_session(std::move(std::make_unique<Session>(Session(id, token))));

    EXPECT_EQ(id, sessionManager_.get_id_by_token(token));
}