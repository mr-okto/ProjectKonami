#include <string.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <DialogueManager.hpp>
#include <MessageManager.hpp>
#include "DbSession.hpp"
#include "UserManager.hpp"
#include "DbConnectorStub.hpp"

using ::testing::_;
using ::testing::Test;

class DbTest : public Test {
 public:
  DbTest() : Test(), test_uname_("John"), test_pwd_( "123456") {
    DbConnectorStub connector;
    session_ = new DbSession<DbConnectorStub>(std::move(std::make_unique<DbConnectorStub>(connector)));
    session_->connect("localhost", 5432, "db_user", "db_pass", "konami_test_db");
  }
  virtual ~DbTest() {
    session_->disconnect();
    delete session_;
  }

  std::string test_uname_;
  std::string test_pwd_;
  DbSession<DbConnectorStub>* session_;
};

TEST(DbSession, DbSession_Connect_Test) {
  DbConnectorStub connector;
  DbSession<DbConnectorStub> session(std::move(std::make_unique<DbConnectorStub>(connector)));
  auto result = session.connect("localhost", 5432, "db_user", "db_pass", "db_name");
  EXPECT_EQ(result, true);
}

TEST_F(DbTest, DbUserManager_CreateUser_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto result = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->username_, test_uname_);
  EXPECT_EQ(result->pwd_hash_, test_pwd_);
}

TEST_F(DbTest, DbUserManager_GetAll_Empty_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto result = user_manager.get_all_users();
  EXPECT_EQ(result.size(), 0);
}

TEST_F(DbTest, DbUserManager_GetAll_NotEmpty_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  user_manager.create_user(test_uname_, test_pwd_);
  auto result = user_manager.get_all_users();
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0]->username_, test_uname_);
  EXPECT_EQ(result[0]->pwd_hash_, test_pwd_);
}

TEST_F(DbTest, DbUserManager_GetUser_ById_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto user = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user, nullptr);
  auto result = user_manager.get_user(user->id_);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->username_, test_uname_);
  EXPECT_EQ(result->pwd_hash_, test_pwd_);
}

TEST_F(DbTest, DbUserManager_GetUser_ById_NonExistent_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto result = user_manager.get_user(0);
  EXPECT_EQ(result, nullptr);
}

TEST_F(DbTest, DbUserManager_GetUser_ByUsername_NonExistent_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto result = user_manager.get_user("non_existent_username");
  EXPECT_EQ(result, nullptr);
}

TEST_F(DbTest, DbUserManager_UpdateUsername_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto user = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user, nullptr);
  std::string new_username = "new_username";
  auto result = user_manager.update_username(user->id_, new_username);
  EXPECT_EQ(result, true);
  EXPECT_EQ(user->username_, new_username);
}

TEST_F(DbTest, DbUserManager_UpdatePwd_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto user = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user, nullptr);
  std::string new_pwd = "new_password";
  auto result = user_manager.update_password(user->id_, new_pwd);
  EXPECT_EQ(result, true);
  EXPECT_EQ(user->pwd_hash_, new_pwd);
}

TEST_F(DbTest, DbUserManager_AddPicture_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto user = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user, nullptr);
  std::string path = "some_path";
  auto result = user_manager.add_picture(user->id_, path, 0);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->path_, path);
  EXPECT_EQ(result->access_lvl_, 0);
  EXPECT_EQ(result->user_, user);
}

TEST_F(DbTest, DbDialogueManager_GetDialogue_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto user_a = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user_a, nullptr);
  auto user_b = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user_b, nullptr);

  DialogueManager<DbConnectorStub> dialogue_manager(*session_);
  auto result = dialogue_manager.get_dialogue(user_a->id_, user_b->id_);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->members_[0], user_a);
  EXPECT_EQ(result->members_[1], user_b);
}

TEST_F(DbTest, DbDialogueManager_GetDialogue_ForNonExistentUsers_Test) {
  DialogueManager<DbConnectorStub> dialogue_manager(*session_);
  auto result = dialogue_manager.get_dialogue(0, 0);
  EXPECT_EQ(result, nullptr);
}

TEST_F(DbTest, DbMessageManager_CreateMsg_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto user_a = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user_a, nullptr);
  auto user_b = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user_b, nullptr);

  DialogueManager<DbConnectorStub> dialogue_manager(*session_);
  auto dialogue = dialogue_manager.get_dialogue(user_a->id_, user_b->id_);
  ASSERT_NE(dialogue, nullptr);

  MessageManager<DbConnectorStub> message_manager(*session_);
  std::string text = "Some text";
  auto result = message_manager.create_msg(dialogue->id_, user_a->id_, text);
  ASSERT_NE(result, nullptr);

  EXPECT_EQ(result->text_, text);
  EXPECT_EQ(result->author_, user_a);
  EXPECT_EQ(result->dialogue_, dialogue);
}

TEST_F(DbTest, DbMessageManager_GetMessages_Test) {
  UserManager<DbConnectorStub> user_manager(*session_);
  auto user_a = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user_a, nullptr);
  auto user_b = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_NE(user_b, nullptr);

  DialogueManager<DbConnectorStub> dialogue_manager(*session_);
  auto dialogue = dialogue_manager.get_dialogue(user_a->id_, user_b->id_);
  ASSERT_NE(dialogue, nullptr);

  MessageManager<DbConnectorStub> message_manager(*session_);
  std::string text = "Some text";
  auto msg = message_manager.create_msg(dialogue->id_, user_a->id_, text);
  ASSERT_NE(msg, nullptr);

  auto result = message_manager.get_latest_messages(dialogue->id_, 0);
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0]->dialogue_, dialogue);
}

TEST_F(DbTest, DbMessageManager_GetMessages_Empty_Test) {
  MessageManager<DbConnectorStub> message_manager(*session_);
  auto result = message_manager.get_latest_messages(0, 0);
  EXPECT_EQ(result.size(), 0);
}

