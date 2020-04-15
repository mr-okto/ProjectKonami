#include <cstring>
#include <gtest/gtest.h>
#include "Client.hpp"
#include "ClientService.hpp"

TEST(Client, Client_NewClient_Test) {
  auto server = ClientService();
  auto client = Client(server);
  auto result = client.log_in("uname", "pwd");
  EXPECT_EQ(result, true);
  EXPECT_EQ(client.logged_in_, true);
  client.log_out();
}

TEST(Client, Client_UpdateDialogues_Test) {
  auto server = ClientService();
  auto client = Client(server);
  auto login = client.log_in("uname", "pwd");
  EXPECT_EQ(login, true);
  client.update_dialogues();
  EXPECT_EQ(client.dialogues_.size(), 0);
  client.log_out();
}

TEST(Client, Client_OpenDialogue_Test) {
  auto server = ClientService();
  auto client = Client(server);
  auto login = client.log_in("uname", "pwd");
  EXPECT_EQ(login, true);
  client.open_dialogue(1);
  EXPECT_NE(client.active_dialogue_, 0);
  client.update_dialogues();
  EXPECT_EQ(client.dialogues_.size(), 1);
  client.log_out();
}


TEST(Client, Client_SendMsg_Test) {
  auto server = ClientService();
  auto client = Client(server);
  auto login = client.log_in("uname", "pwd");
  EXPECT_EQ(login, true);
  client.open_dialogue(1);
  EXPECT_NE(client.active_dialogue_, 0);
  client.send_message("text");
  EXPECT_EQ(client.messages_.size(), 1);
  client.log_out();
}

TEST(Client, Client_ChatEvent_Msg_Test) {
  auto server = ClientService();
  auto client = Client(server);
  ChatEvent event;
  event.action = NEW_MSG;
  event.data = "Msg";
  event.object_id = 1;
  event.user_id = 1;
  client.handle_event(event);
  EXPECT_EQ(client.messages_.size(), 1);
}