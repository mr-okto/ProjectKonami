#include <string.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Wt/Dbo/backend/Postgres.h>
#include "DialogueManager.hpp"
#include "MessageManager.hpp"
#include "DbSession.hpp"
#include "UserManager.hpp"
#include "DbConnectorStub.hpp"
#include "Randomizer.hpp"

using ::testing::_;
using ::testing::Test;
using Wt::Dbo::backend::Postgres;

class DbTest : public Test {
 public:
  DbTest() : Test(), session_(), test_uname_(Randomizer::get_instance().get_string(64)), test_pwd_( "123456") {
    assert(session_.connect("db_conf.json"));
  }
  virtual ~DbTest() {
    session_.disconnect();
  }

  std::string test_uname_;
  std::string test_pwd_;
  DbSession<Postgres> session_;
};

TEST(DbSession, DbSession_Connect_Test) {
  DbSession<Postgres> session;
  auto result = session.connect("db_conf.json");
  EXPECT_EQ(result, true);
}

TEST_F(DbTest, DbUserManager_CreateUser_Test) {
  UserManager<Postgres> user_manager(session_);
  auto result = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!result, false);
  EXPECT_EQ(result->username_, test_uname_);
  EXPECT_EQ(result->pwd_hash_, test_pwd_);
}

TEST_F(DbTest, DbUserManager_GetAll_NotEmpty_Test) {
  UserManager<Postgres> user_manager(session_);
  auto pre_count = user_manager.get_all_users().size();
  auto new_user = user_manager.create_user(test_uname_, test_pwd_);
  auto post_count = user_manager.get_all_users().size();
  ASSERT_EQ(post_count - pre_count, 1);
  EXPECT_EQ(new_user->username_, test_uname_);
  EXPECT_EQ(new_user->pwd_hash_, test_pwd_);
}

TEST_F(DbTest, DbUserManager_GetUser_ById_Test) {
  UserManager<Postgres> user_manager(session_);
  auto user = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user, false);
  auto result = user_manager.get_user(user.id());
  ASSERT_EQ(!result, false);
  EXPECT_EQ(result->username_, test_uname_);
  EXPECT_EQ(result->pwd_hash_, test_pwd_);
}

TEST_F(DbTest, DbUserManager_GetUser_ById_NonExistent_Test) {
  UserManager<Postgres> user_manager(session_);
  auto result = user_manager.get_user(0);
  EXPECT_EQ(!result, true);
}

TEST_F(DbTest, DbUserManager_GetUser_ByUsername_NonExistent_Test) {
  UserManager<Postgres> user_manager(session_);
  auto result = user_manager.get_user("non_existent_username");
  EXPECT_EQ(!result, true);
}

TEST_F(DbTest, DbUserManager_UpdateUsername_Test) {
  UserManager<Postgres> user_manager(session_);
  auto user = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user, false);
  std::string new_username = "new_username";
  auto result = user_manager.update_username(user.id(), new_username);
  EXPECT_EQ(result, true);
  EXPECT_EQ(user->username_, new_username);
}

TEST_F(DbTest, DbUserManager_UpdatePwd_Test) {
  UserManager<Postgres> user_manager(session_);
  auto user = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user, false);
  std::string new_pwd = "new_password";
  auto result = user_manager.update_password(user.id(), new_pwd);
  EXPECT_EQ(result, true);
  EXPECT_EQ(user->pwd_hash_, new_pwd);
}

TEST_F(DbTest, DbUserManager_AddPicture_Test) {
  UserManager<Postgres> user_manager(session_);
  auto user = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user, false);
  std::string path = "some_path";
  auto result = user_manager.add_picture(user.id(), path, 0);
  ASSERT_NE(!result, true);
  EXPECT_EQ(result->path_, path);
  EXPECT_EQ(result->access_lvl_, 0);
  EXPECT_EQ(result->user_, user);
}

TEST_F(DbTest, DbDialogueManager_GetDialogue_Test) {
  UserManager<Postgres> user_manager(session_);
  auto user_a = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user_a, false);
  std::reverse(test_uname_.begin(), test_uname_.end());
  auto user_b = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user_b, false);

  DialogueManager<Postgres> dialogue_manager(session_);
  auto result = dialogue_manager.get_or_create_dialogue(user_a.id(), user_b.id());
  ASSERT_EQ(std::get<1>(result), true);
  DialogueModelPtr dialogue = std::get<0>(result);
  ASSERT_EQ(!dialogue, false);
  bool a_is_member = false;
  bool b_is_member = false;
  session_.start_transaction();
  for (const auto &member : dialogue->members_) {
    if (member.id() == user_a.id()) {
      a_is_member = true;
    } else if (member.id() == user_b.id()) {
      b_is_member = true;
    }
  }
  session_.end_transaction();
  EXPECT_TRUE(a_is_member);
  EXPECT_TRUE(b_is_member);
}

TEST_F(DbTest, DbDialogueManager_GetDialogue_ForNonExistentUsers_Test) {
  DialogueManager<Postgres> dialogue_manager(session_);
  auto result = dialogue_manager.get_dialogue(0, 0);
  EXPECT_EQ(!result, true);
}

TEST_F(DbTest, DbMessageManager_CreateMsg_Test) {
  UserManager<Postgres> user_manager(session_);
  auto user_a = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user_a, false);
  std::reverse(test_uname_.begin(), test_uname_.end());
  auto user_b = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user_b, false);

  DialogueManager<Postgres> dialogue_manager(session_);
  auto result = dialogue_manager.get_or_create_dialogue(user_a.id(), user_b.id());
  ASSERT_EQ(std::get<1>(result), true);
  DialogueModelPtr dialogue = std::get<0>(result);
  ASSERT_EQ(!dialogue, false);

  MessageManager<Postgres> message_manager(session_);
  std::string text = "Some text";
  auto msg = message_manager.create_msg(dialogue.id(), user_a.id(), text);
  ASSERT_EQ(!msg, false);
  EXPECT_EQ(msg->text_, text);
  EXPECT_EQ(msg->author_, user_a);
  EXPECT_EQ(msg->dialogue_, dialogue);
}

TEST_F(DbTest, DbMessageManager_GetMessages_Test) {
  UserManager<Postgres> user_manager(session_);
  auto user_a = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user_a, false);
  std::reverse(test_uname_.begin(), test_uname_.end());
  auto user_b = user_manager.create_user(test_uname_, test_pwd_);
  ASSERT_EQ(!user_b, false);

  DialogueManager<Postgres> dialogue_manager(session_);
  auto result = dialogue_manager.get_or_create_dialogue(user_a.id(), user_b.id());
  ASSERT_EQ(std::get<1>(result), true);
  DialogueModelPtr dialogue = std::get<0>(result);
  ASSERT_EQ(!dialogue, false);

  MessageManager<Postgres> message_manager(session_);
  std::string text = "Some text";
  auto msg = message_manager.create_msg(dialogue.id(), user_a.id(), text);
  ASSERT_EQ(!msg, false);

  auto messages = message_manager.get_latest_messages(dialogue.id(), 0);
  ASSERT_EQ(messages.size(), 1);
  EXPECT_EQ(messages.front()->dialogue_, dialogue);
}

TEST_F(DbTest, DbMessageManager_GetMessages_Empty_Test) {
  MessageManager<Postgres> message_manager(session_);
  auto result = message_manager.get_latest_messages(0, 0);
  EXPECT_EQ(result.size(), 0);
}

