#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

class UserManager {
 public:
  DbSession &db_session_;
  explicit UserManager(DbSession &db_session);
  std::vector<UserModelPtr> get_all_users();
  UserModelPtr create_user(const std::string &username, const std::string &pwd_hash);
  UserModelPtr get_user(uint32_t id);
  UserModelPtr get_user(const std::string &username);
  bool update_username(uint32_t user_id, const std::string &username);
  bool update_password(uint32_t user_id, const std::string &pwd_hash);
  PictureModelPtr add_picture(uint32_t user_id, const std::string &path, uint32_t access_lvl);
};

