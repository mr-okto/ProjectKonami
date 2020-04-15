#include "Client.hpp"

Client::Client(Server &server) : server_(server) {}

Client::~Client() {}

bool Client::log_in(const std::string &username, const std::string &password) {
  return false;
}

void Client::log_out() {

}

void Client::update_dialogues() {

}

bool Client::update_messages(uint32_t dialogue_id) {
  return false;
}

void Client::handle_event(const ChatEvent &event) {

}

bool Client::send_message(const std::string &text) {
  return false;
}

bool Client::open_dialogue(uint32_t) {
  return false;
}
