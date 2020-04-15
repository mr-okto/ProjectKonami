#pragma once

#include <cstdint>
#include <string>
#include "ChatEvent.hpp"
#include "DataTypesStub.hpp"

class Client;

class ClientService {
 public:
  ClientService();
  ClientService(const ClientService&) = delete;
  ClientService &operator=(const ClientService &) = delete;
  bool connect(Client *client, ChatEventCallback event_callback);
  bool disconnect(Client *client);
  std::string get_session_token(const std::string &username, const std::string &password);
  bool close_session(const std::string &token);
  uint32_t open_dialogue(const std::string &session_token, uint32_t other_user);
  bool send_message(const std::string &session_token, uint32_t dialogue, const std::string &text);
  std::vector<Dialogue> get_dialogues(const std::string &session_token);
  std::vector<Message> get_messages(const std::string &session_token, uint32_t dialogue_id);
};
