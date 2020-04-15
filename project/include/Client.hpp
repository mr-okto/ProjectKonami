#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ChatEvent.hpp"

class Server;
struct Message;
struct Dialogue;

class Client {
 private:
  Server &server_;
  bool logged_in_;
  std::string session_token_;
 public:
  uint32_t user_id_;
  std::vector<Dialogue> dialogues_;
  uint32_t active_dialogue_;
  std::vector<Message> messages_;
  Client(Server &server);
  ~Client();
  bool log_in(const std::string &username, const std::string &password);
  void log_out();
  void get_dialogues();
  void get_messages(uint32_t dialogue_id);
  void send_message(const std::string &text);
  void processChatEvent(const ChatEvent& event);
};

