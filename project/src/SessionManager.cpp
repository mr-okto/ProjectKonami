#include "SessionManager.hpp"

class Client;

std::vector<uint32_t> SessionManager::get_online_users() {
    return std::vector<uint32_t>();
}

uint32_t SessionManager::get_id_by_token(std::string &token) {
    return 0;
}

bool SessionManager::has(Client *) {
    return false;
}
