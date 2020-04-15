#pragma once
#include <string>
#include <vector>
#include <functional>

namespace chat {
class UserManager {};
class PhotoManager {};
class SessionManager {};
class UserModel {};


struct Picture {};
struct User {
    unsigned int user_id;
    std::string user_name;
    Picture picture;
};

class UserService {
 public:
    UserService(UserManager& user_manager, PhotoManager& photo_manager, SessionManager& session_manager);
    std::vector<User> get_all_users();
    std::vector<User> get_online_users(unsigned int user_id);
    User update_user(const User& user);
    Picture get_picture(unsigned int fuser_id, unsigned int suser_id);
 private:
    UserManager& user_manager_;
    PhotoManager& photo_manager_;
    SessionManager& session_manager_;
};
}