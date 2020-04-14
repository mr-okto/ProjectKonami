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
        Session default_session(0, "default");
        sessionManager_.add_session(default_session);

    }

    void TearDown() override {

    }

    SessionManager sessionManager_;
};


TEST_F(SessionManagerTest, AddSessionTest) {
    Session session(1, "token");

    EXPECT_TRUE(sessionManager_.add_session(session));
    EXPECT_FALSE(sessionManager_.add_session(session)); // сессии уникальны, нельзя 2 раза добавить одну и ту же сессию
}

TEST_F(SessionManagerTest, CloseSessionTest) {
    Session session(1, "token");

    EXPECT_TRUE(sessionManager_.close_session(session));
    EXPECT_FALSE(sessionManager_.close_session(session));
}

TEST_F(SessionManagerTest, ValidateSessionTest) {
    Session session(1, "token");

    EXPECT_FALSE(sessionManager_.validate_session(session.token()));
    EXPECT_TRUE(sessionManager_.validate_session(Session(0, "default").token()));
}
