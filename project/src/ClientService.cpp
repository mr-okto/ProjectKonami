#include "ClientService.hpp"

ClientService::ClientService() {

}

bool ClientService::connect(Client *client, ChatEventCallback event_callback) {
  return false;
}

bool ClientService::disconnect(Client *client) {
  return false;
}

std::string ClientService::get_session_token(const std::string &username, const std::string &password) {
  return std::string();
}

bool ClientService::close_session(const std::string &token) {
  return false;
}

bool ClientService::send_message(const std::string &session_token, uint32_t dialogue, const std::string &text) {
  return false;
}

std::vector<Dialogue> ClientService::get_dialogues(const std::string &session_token) {
  return std::vector<Dialogue>();
}

std::vector<Message> ClientService::get_messages(const std::string &session_token, uint32_t dialogue_id) {
  return std::vector<Message>();
}

uint32_t ClientService::open_dialogue(const std::string &session_token, uint32_t other_user) {
  return 0;
}
