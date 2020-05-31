#include "SessionManager.hpp"

class Client;

std::vector<uint32_t> SessionManager::get_online_users() {
    return std::vector<uint32_t>();
}

uint32_t SessionManager::get_id_by_token(std::string &token) {
    return 0;
}

bool SessionManager::has_reserved(const std::string &username) {
    return reserved_map_.find(username) != reserved_map_.end();
}

void SessionManager::reserve(const std::string& username, uint32_t user_id) {
    if (reserved_map_.find(username) == reserved_map_.end()) {
        reserved_map_.insert(std::pair<std::string, uint32_t>(username, user_id));
    }
}

void SessionManager::unreserve(const std::string& username) {
    auto it = reserved_map_.find(username);
    if (it != reserved_map_.end()) {
        reserved_map_.erase(it);
    }
}

bool SessionManager::add_session(Client *client, const Session &s) {
    if (active_sessions_.count(client) == 0) {
        active_sessions_[client] = s;
        return true;
    } else
        return false;
}

bool SessionManager::close_session(Client *client) {
    return active_sessions_.erase(client) == 1;
}

SessionManager::SessionManager() : scheduler_() {

}

void SessionManager::set_cookie(const std::string &username, const std::string &cookie) {
    for (auto& session : active_sessions_) {
        if (session.second.username_ == username) {
            if (session.second.cookie_.empty()) {
                session.second.cookie_ = cookie;
            }
        }
    }
}
