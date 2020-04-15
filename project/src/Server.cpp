#include "Server.hpp"

Server::Server() {

}

bool Server::connect(Client *client, ChatEventCallback event_callback) {
  return false;
}

bool Server::disconnect(Client *client) {
  return false;
}

std::string Server::get_session_token(const std::string &username, const std::string &password) {
  return std::string();
}

bool Server::close_session(const std::string &token) {
  return false;
}

bool Server::send_message(const std::string &session_token, uint32_t dialogue, const std::string &text) {
  return false;
}

std::vector<Dialogue> Server::get_dialogues(const std::string &session_token) {
  return std::vector<Dialogue>();
}

std::vector<Message> Server::get_messages(const std::string &session_token, uint32_t dialogue_id) {
  return std::vector<Message>();
}

uint32_t Server::open_dialogue(const std::string &session_token, uint32_t other_user) {
  return 0;
}
