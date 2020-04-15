#include "Client.hpp"

Client::Client(Server &server) : server_(server) {}

Client::~Client() {}

bool Client::log_in(const std::string &username, const std::string &password) {
  return false;
}

void Client::log_out() {

}

void Client::get_dialogues() {

}

void Client::get_messages(uint32_t dialogue_id) {

}

void Client::processChatEvent(const ChatEvent &event) {

}

void Client::send_message(const std::string &text) {

}
