#include "UserManager.hpp"
UserManager::UserManager(DbSession &db_session)
    : db_session_(db_session) {}

std::vector<UserModelPtr> UserManager::get_all_users() {
  return std::vector<UserModelPtr>();
}

UserModelPtr UserManager::create_user(const std::string &username, const std::string &pwd_hash) {
  return nullptr;
}

UserModelPtr UserManager::get_user(uint32_t id) {
  return nullptr;
}

UserModelPtr UserManager::get_user(const std::string &username) {
  return nullptr;
}

bool UserManager::update_username(uint32_t user_id, const std::string &username) {
  return false;
}

bool UserManager::update_password(uint32_t user_id, const std::string &username) {
  return false;
}

PictureModel *UserManager::add_picture(uint32_t user_id, const std::string &path, uint32_t access_lvl) {
  return nullptr;
}
