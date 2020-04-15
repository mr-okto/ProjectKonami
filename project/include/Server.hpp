#pragma once

#include <cstdint>
#include <string>
#include "ChatEvent.hpp"

class Client;

class Server {
 public:
  Server();
  Server(const Server&) = delete;
  Server &operator=(const Server &) = delete;
  bool connect(Client *client, const ChatEventCallback& handleEvent);
  bool disconnect(Client *client);
  std::string get_session_token(const std::string &username, const std::string &password);
  void close_session(const std::string &token);
  bool send_message(const std::string &session_token, uint32_t dialogue, const std::string &text);
};
