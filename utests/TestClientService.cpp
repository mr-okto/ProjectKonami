#include <cstring>
#include <gtest/gtest.h>
#include "Client.hpp"
#include "ClientService.hpp"

TEST(ClientService, ClientService_NewSession_Test) {
  auto server = ClientService();
  auto client = Client(server);
  auto result = server.get_session_token("uname", "pwd");
  EXPECT_GT(result.size(), 0);
}

TEST(ClientService, ClientService_CloseSession_Valid_Test) {
  auto server = ClientService();
  auto client = Client(server);
  auto token = server.get_session_token("uname", "pwd");
  bool result = server.close_session(token);
  EXPECT_EQ(result, true);
}

TEST(ClientService, ClientService_CloseSession_invalid_Test) {
  auto server = ClientService();
  bool result = server.close_session("InvalidToken");
  EXPECT_EQ(result, false);
}

TEST(ClientService, ClientService_SendMessage_Valid_Test) {
  auto server = ClientService();
  auto token = server.get_session_token("uname", "pwd");
  auto dialogue = server.open_dialogue(token, 1);
  auto msg_list = server.get_messages(token, dialogue);
  int count = msg_list.size();
  bool result = server.send_message(token, dialogue, "Some text");
  EXPECT_EQ(result, true);
  msg_list = server.get_messages(token, dialogue);
  count += 1;
  EXPECT_EQ(msg_list.size(), count);
  server.close_session(token);
}

TEST(ClientService, ClientService_SendMessage_Inalid_Test) {
  auto server = ClientService();
  bool result = server.send_message("Invalid token", 1, "Some text");
  EXPECT_EQ(result, false);
}

TEST(ClientService, ClientService_GetDialogues_Test) {
  auto server = ClientService();
  auto client = Client(server);
  auto token = server.get_session_token("uname", "pwd");
  auto dialogues = server.get_dialogues(token);
  int count = dialogues.size();
  auto dialogue = server.open_dialogue(token, 1);
  dialogues = server.get_dialogues(token);
  count += 1;
  EXPECT_EQ(dialogues.size(), count);
  server.close_session(token);
}
