#include "UserService.hpp"

namespace chat {
std::vector<User> UserService::get_all_users() {
    return std::vector<User>();
}

std::vector<User> UserService::get_online_users(unsigned int user_id) {
    return std::vector<User>();
}

User UserService::edit_user(const User& user) {
    return User();
}
}