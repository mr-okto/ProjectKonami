#include "UserService.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockSessionManager : public chat::SessionManager {
 public:
    MOCK_METHOD0(get_online_users, std::vector<unsigned int>());
};

class MockPhotoManager : public chat::PhotoManager {
 public:
    MOCK_METHOD2(get_picture, chat::Picture(unsigned int first_id, unsigned int second_id));
};

class MockUserManager : public chat::UserManager {
 public:
    MOCK_METHOD0(get_all_users, std::vector<chat::UserModel*>());
    MOCK_METHOD2(update_username, bool(unsigned int user_id, std::string username));
    MOCK_METHOD2(update_password, bool(unsigned int user_id, std::string password));
};

TEST(USGetOnlineUsers, ok) {
    MockSessionManager object;
    EXPECT_CALL(object, get_online_users()).Times(testing::AtLeast(1));

    chat::PhotoManager photo;
    chat::UserManager user;

    chat::UserService u_s(user, photo, object);
    u_s.get_online_users(4);
}

TEST(USGetAllUsers, ok) {
    MockUserManager object;
    EXPECT_CALL(object, get_all_users).Times(testing::AtLeast(1));

    chat::PhotoManager photo;
    chat::SessionManager session;

    chat::UserService u_s(object, photo, session);
    u_s.get_all_users();
}

TEST(USGetPicture, ok) {
    MockPhotoManager object;
    EXPECT_CALL(object, get_picture(1, 2)).Times(testing::AtLeast(1));

    chat::UserManager user;
    chat::SessionManager session;

    chat::UserService u_s(user, object, session);
    u_s.get_picture(1, 2);
}

TEST(USUpdateProfile, ok) {
    MockUserManager object;
    chat::User user = {.user_id=1, .user_name="123"};
    EXPECT_CALL(object, update_username(user.user_id, user.user_name)).Times(testing::AtLeast(1));
    EXPECT_CALL(object, update_password(user.user_id, user.user_name)).Times(testing::AtLeast(1));

    chat::PhotoManager photo;
    chat::SessionManager session;

    chat::UserService u_s(object, photo, session);
    u_s.update_user(user);
}