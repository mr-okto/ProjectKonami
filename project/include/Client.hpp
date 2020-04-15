#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ChatEvent.hpp"

#include "DataTypesStub.hpp"

class ClientService;

class Client {
 private:
  ClientService &client_service_;
  std::string session_token_;
 public:
  bool logged_in_;
  std::vector<Dialogue> dialogues_;
  uint32_t active_dialogue_;
  std::vector<Message> messages_;
  explicit Client(ClientService &server);
  Client(const Client&) = delete;
  Client &operator=(const Client&) = delete;
  ~Client();
  bool log_in(const std::string &username, const std::string &password);
  void log_out();
  void update_dialogues();
  bool update_messages();
  bool open_dialogue(uint32_t);
  bool send_message(const std::string &text);
  void handle_event(const ChatEvent& event);
};

