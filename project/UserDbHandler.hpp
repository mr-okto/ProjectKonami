#pragma once

#include "DbSession.hpp"
#include "DbData.hpp"

template< class DBConnector >
class UserDbHandler {
 public:
  DbSession<DBConnector> &db_session_;
  explicit UserDbHandler(DbSession<DBConnector> &db_session);
  Users get_all_users();
  UserPtr create_user(const std::string &username, const std::string &pwd_hash);
  UserPtr get_user(IdType id);
  UserPtr get_user(const std::string &username);
  bool update_username(IdType user_id, const std::string &username);
  bool update_password(IdType user_id, const std::string &pwd_hash);
  PicturePtr add_picture(IdType user_id, const std::string &path, int access_lvl);
  PicturePtr add_picture(const UserPtr &user, const std::string &path, int access_lvl);
  Pictures get_pictures(IdType user_id);
  bool hide_pictures(IdType user_id);
};

template< class DBConnector >
UserDbHandler<DBConnector>::UserDbHandler(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template< class DBConnector >
Users UserDbHandler<DBConnector>::get_all_users() {
  db_session_.start_transaction();
  Users result = db_session_.template find<UserDbData>().orderBy("username");
  db_session_.end_transaction();
  return result;
}

template< class DBConnector >
UserPtr UserDbHandler<DBConnector>::create_user(const std::string &username, const std::string &pwd_hash) {
  if (get_user(username)) {
    // Username already exists
    return UserPtr();
  }
  auto new_user = std::make_unique<UserDbData>(UserDbData());
  new_user->username_ = username;
  new_user->pwd_hash_ = pwd_hash;
  return db_session_.add(std::move(new_user));
}

template< class DBConnector >
UserPtr UserDbHandler<DBConnector>::get_user(IdType id) {
  return db_session_.template get_by_id<UserDbData>(id);
}

template< class DBConnector >
UserPtr UserDbHandler<DBConnector>::get_user(const std::string &username) {
  db_session_.start_transaction();
  UserPtr user = db_session_.template find<UserDbData>().where("username = ?").bind(username);
  db_session_.end_transaction();
  return user;
}

template< class DBConnector >
bool UserDbHandler<DBConnector>::update_username(IdType user_id, const std::string &username) {
  db_session_.start_transaction();
  UserPtr user = get_user(user_id);
  bool result = (bool) user;
  if (result) {
    user.modify()->username_ = username;
  }
  db_session_.end_transaction();
  return result;
}

template< class DBConnector >
bool UserDbHandler<DBConnector>::update_password(IdType user_id, const std::string &pwd_hash) {
  db_session_.start_transaction();
  UserPtr user = get_user(user_id);
  bool result = (bool) user;
  if (result) {
    user.modify()->pwd_hash_ = pwd_hash;
  }
  db_session_.end_transaction();
  return result;
}

template< class DBConnector >
PicturePtr UserDbHandler<DBConnector>::add_picture(IdType user_id, const std::string &path, int access_lvl) {
  UserPtr user = get_user(user_id);
  if (!user) {
    return nullptr;
  }
  return add_picture(user, path, access_lvl);
}

template< class DBConnector >
PicturePtr UserDbHandler<DBConnector>::add_picture(const UserPtr &user, const std::string &path, int access_lvl) {
  auto new_picture = std::make_unique<PictureDbData>(PictureDbData());
  new_picture->path_ = path;
  new_picture->access_lvl_ = access_lvl;

  db_session_.start_transaction();
  new_picture->user_ = user;
  PicturePtr result = db_session_.add(std::move(new_picture));
  db_session_.end_transaction();
  return result;
}

template<class DBConnector>
Pictures UserDbHandler<DBConnector>::get_pictures(IdType user_id) {
  db_session_.start_transaction();
  UserPtr user = get_user(user_id);
  if (!user) {
    return Pictures();
  }
  Pictures pictures = db_session_.template find<PictureDbData>().where("user_id = ?").bind(user.id())
      .where("access_lvl >= ?").bind(0);
  db_session_.end_transaction();
  return pictures;
}

template<class DBConnector>
bool UserDbHandler<DBConnector>::hide_pictures(IdType user_id) {
  Pictures pictures = get_pictures(user_id);
  if (pictures.empty()) {
    return false;
  }
  db_session_.start_transaction();
  for (auto &pic: pictures) {
    pic.modify()->access_lvl_ = -1;
  }
  db_session_.end_transaction();
  return true;
}
