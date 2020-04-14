#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

template< class DBConnector >
class UserManager {
 public:
  DbSession<DBConnector> &db_session_;
  explicit UserManager(DbSession<DBConnector> &db_session);
  std::vector<UserModelPtr> get_all_users();
  UserModelPtr create_user(const std::string &username, const std::string &pwd_hash);
  UserModelPtr get_user(uint32_t id);
  UserModelPtr get_user(const std::string &username);
  bool update_username(uint32_t user_id, const std::string &username);
  bool update_password(uint32_t user_id, const std::string &pwd_hash);
  PictureModelPtr add_picture(uint32_t user_id, const std::string &path, uint32_t access_lvl);
};

template< class DBConnector >
UserManager<DBConnector>::UserManager(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template< class DBConnector >
std::vector<UserModelPtr> UserManager<DBConnector>::get_all_users() {
  return std::vector<UserModelPtr>();
}

template< class DBConnector >
UserModelPtr UserManager<DBConnector>::create_user(const std::string &username, const std::string &pwd_hash) {
  return nullptr;
}

template< class DBConnector >
UserModelPtr UserManager<DBConnector>::get_user(uint32_t id) {
  return nullptr;
}

template< class DBConnector >
UserModelPtr UserManager<DBConnector>::get_user(const std::string &username) {
  return nullptr;
}

template< class DBConnector >
bool UserManager<DBConnector>::update_username(uint32_t user_id, const std::string &username) {
  return false;
}

template< class DBConnector >
bool UserManager<DBConnector>::update_password(uint32_t user_id, const std::string &username) {
  return false;
}

template< class DBConnector >
PictureModelPtr UserManager<DBConnector>::add_picture(uint32_t user_id, const std::string &path, uint32_t access_lvl) {
  return nullptr;
}


