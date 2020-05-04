#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

template < class DBConnector >
class MessageManager {
 public:
  DbSession<DBConnector> &db_session_;
  explicit MessageManager(DbSession<DBConnector> &db_session);
  MessageModelPtr create_msg(IdType dialogue_id,
                             IdType author_id, const std::string &text);
  Messages get_latest_messages(IdType dialogue_id, time_t start);
};

template < class DBConnector >
MessageManager<DBConnector>::MessageManager(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template < class DBConnector >
MessageModelPtr MessageManager<DBConnector>::create_msg(IdType dialogue_id, IdType author_id,
                                                        const std::string &text) {
  auto new_msg = std::make_unique<MessageModel>(MessageModel());
  new_msg->text_ = text;
  new_msg->creation_dt_ = std::time(nullptr);
  new_msg->is_read_ = false;
  db_session_.start_transaction();
  new_msg->author_ = db_session_.template get_by_id<UserModel>(author_id);
  new_msg->dialogue_ = db_session_.template get_by_id<DialogueModel>(dialogue_id);
  MessageModelPtr result = db_session_.add(std::move(new_msg));
  db_session_.commit_transaction();
  return result;
}

template < class DBConnector >
Messages MessageManager<DBConnector>::get_latest_messages(IdType dialogue_id, time_t start) {
  Messages result = db_session_.template find<MessageModel>().where("dialogue_id = ?").bind(dialogue_id)
                                                             .where("time >= ?").bind(start);
  return result;
}
