#pragma once
#include <string>
#include <vector>
#include <functional>

namespace chat {
struct User {
    unsigned int user_id;
    std::string user_name;
    std::string picture_path;
};

struct Session {
    unsigned int session_id;
    unsigned int user_id;
    std::function<void (User)> event_call_back;  // FIXME
};

class UserService {
 public:
    std::vector<User> get_all_users();
    std::vector<User> get_online_users(unsigned int user_id);
    User edit_user(const User& user);
 private:
    // FIXME
};
}