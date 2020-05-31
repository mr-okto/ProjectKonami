#include <iostream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Auth.hpp"
#include "SessionManager.hpp"
#include "Session.hpp"

using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::InSequence;


class MockTokenGenerator : public ITokenGenerator {
public:
    MOCK_METHOD(std::string, generate_token, ());
};

class MockSessionManager : public SessionManager {
public:
    MOCK_METHOD(bool, close_session, (const Session&));
    MOCK_METHOD(bool, add_session, (Session&&));
};

class MockUserManager : public UserManager {
public:
    MOCK_METHOD(UserModelPtr, get_user, (const std::string&));
    MOCK_METHOD(UserModelPtr, get_user, (uint32_t));
};

class AuthTest : public ::testing::Test {
protected:
    void SetUp() override {
        Session default_session(0, "default");
        sessionManager_.add_session(std::move(default_session));
    }

    void TearDown() override {

    }

    MockUserManager userManager_;
    MockTokenGenerator tokenGenerator_;
    MockSessionManager sessionManager_;
};


TEST_F(AuthTest, CanProcessAuth) {
    {
        InSequence s;

        EXPECT_CALL(userManager_, get_user("login")).Times(AtLeast(1));
        EXPECT_CALL(tokenGenerator_, generate_token()).Times(AtLeast(1));
    }

    Auth auth(std::ref(userManager_), &tokenGenerator_, &sessionManager_);
    EXPECT_NE(std::string(), auth.sign_in(AuthData("login", "password")));
}

class MockAuth : public Auth {
public:
    explicit MockAuth(UserManager& u, ITokenGenerator* t, SessionManager* s) : Auth(u, t, s){};
    MOCK_METHOD(bool, check_data_from_db, ());
};

TEST_F(AuthTest, MustCheckAuthDataFromDB) {
    MockAuth mockAuth(std::ref(userManager_), &tokenGenerator_, &sessionManager_);
    EXPECT_CALL(mockAuth, check_data_from_db()).Times(AtLeast(2));
    EXPECT_CALL(userManager_, get_user("123")).Times(AtLeast(2));
    EXPECT_CALL(tokenGenerator_, generate_token()).Times(AtLeast(1));


    mockAuth.sign_up(AuthData("123", "123"));
    mockAuth.sign_in(AuthData("123", "123"));
}

//class UserModel;
//
//class UserManager {
//public:
//    virtual UserModel* get_user(std::string) = 0;
//};
//
//class MockUserManager : public UserManager {
//public:
//    MOCK_METHOD(UserModel*, get_user, (std::string));
//};

