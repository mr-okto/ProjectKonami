#include "UserService.hpp"

namespace chat {
UserService::UserService(UserManager& user_manager, PhotoManager& photo_manager, SessionManager& session_manager)
    : user_manager_(user_manager), 
      photo_manager_(photo_manager),
      session_manager_(session_manager) {}

std::vector<User> UserService::get_all_users() {
    return std::vector<User>();
}

std::vector<User> UserService::get_online_users(unsigned int user_id) {
    return std::vector<User>();
}

User UserService::update_user(const User& user) {
    return User();
}

Picture UserService::get_picture(unsigned int fuser_id, unsigned int suser_id) {
    return Picture();
}
}