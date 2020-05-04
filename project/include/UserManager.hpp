#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

template< class DBConnector >
class UserManager {
 public:
  DbSession<DBConnector> &db_session_;
  explicit UserManager(DbSession<DBConnector> &db_session);
  Users get_all_users();
  UserModelPtr create_user(const std::string &username, const std::string &pwd_hash);
  UserModelPtr get_user(IdType id);
  UserModelPtr get_user(const std::string &username);
  void update_username(IdType user_id, const std::string &username);
  void update_password(IdType user_id, const std::string &pwd_hash);
  PictureModelPtr add_picture(IdType user_id, const std::string &path, int access_lvl);
};

template< class DBConnector >
UserManager<DBConnector>::UserManager(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template< class DBConnector >
Users UserManager<DBConnector>::get_all_users() {
  Users result = db_session_.template find<UserModel>().orderBy("username");
  return result;
}

template< class DBConnector >
UserModelPtr UserManager<DBConnector>::create_user(const std::string &username, const std::string &pwd_hash) {
  auto new_user = std::make_unique<UserModel>(UserModel());
  new_user->username_ = username;
  new_user->pwd_hash_ = pwd_hash;
  return db_session_.add(std::move(new_user));
}

template< class DBConnector >
UserModelPtr UserManager<DBConnector>::get_user(IdType id) {
  return db_session_.template get_by_id<UserModel>(id);
}

template< class DBConnector >
UserModelPtr UserManager<DBConnector>::get_user(const std::string &username) {
  return db_session_.template find<UserModel>().where("username = ?").bind(username);
}

template< class DBConnector >
void UserManager<DBConnector>::update_username(IdType user_id, const std::string &username) {
  db_session_.start_transaction();
  UserModelPtr user = get_user(user_id);
  user.modify()->username_ = username;
  db_session_.commit_transaction();
}

template< class DBConnector >
void UserManager<DBConnector>::update_password(IdType user_id, const std::string &pwd_hash) {
  db_session_.start_transaction();
  UserModelPtr user = get_user(user_id);
  user.modify()->pwd_hash_ = pwd_hash;
  db_session_.commit_transaction();
}

template< class DBConnector >
PictureModelPtr UserManager<DBConnector>::add_picture(IdType user_id, const std::string &path, int access_lvl) {
  auto new_picture = std::make_unique<PictureModel>(PictureModel());
  new_picture->path_ = path;
  new_picture->access_lvl_ = access_lvl;

  db_session_.start_transaction();
  new_picture->user_ = db_session_.template get_by_id<UserModel>(user_id);
  PictureModelPtr result = db_session_.add(std::move(new_picture));
  db_session_.commit_transaction();
  return result;
}
