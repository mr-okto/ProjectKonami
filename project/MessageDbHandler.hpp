#pragma once

#include "DbSession.hpp"
#include "DbData.hpp"

template < class DBConnector >
class MessageDbHandler {
 public:
  DbSession<DBConnector> &db_session_;
  explicit MessageDbHandler(DbSession<DBConnector> &db_session);
  MessagePtr create_msg(IdType dialogue_id,
                        IdType author_id, const std::string &text);
  Messages get_latest_messages(IdType dialogue_id, time_t start);
  MessagePtr get_last_msg(IdType dialogue_id);
  bool mark_read(IdType msg_id);
  void mark_read(MessagePtr &msg);
  ContentPtr add_content(IdType msg_id, ContentDbData::Type type, const std::string &path,
                         const std::string &metadata = {});
  ContentPtr add_content(MessagePtr &msg, ContentDbData::Type type, const std::string &path,
                         const std::string &metadata = {});
};

template < class DBConnector >
MessageDbHandler<DBConnector>::MessageDbHandler(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template < class DBConnector >
MessagePtr MessageDbHandler<DBConnector>::create_msg(IdType dialogue_id, IdType author_id,
                                                     const std::string &text) {
  auto new_msg = std::make_unique<MessageDbData>(MessageDbData());
  new_msg->text_ = text;
  new_msg->creation_dt_ = std::time(nullptr);
  new_msg->is_read_ = false;
  db_session_.start_transaction();
  new_msg->author_ = db_session_.template get_by_id<UserDbData>(author_id);
  new_msg->dialogue_ = db_session_.template get_by_id<DialogueDbData>(dialogue_id);
  MessagePtr result = db_session_.add(std::move(new_msg));
  db_session_.end_transaction();
  return result;
}

template < class DBConnector >
Messages MessageDbHandler<DBConnector>::get_latest_messages(IdType dialogue_id, time_t start) {
  db_session_.start_transaction();
  Messages result = db_session_.template find<MessageDbData>().where("dialogue_id = ?").bind(dialogue_id)
                                                             .where("creation_dt >= ?").bind(start)
                                                             .orderBy("creation_dt");
  db_session_.end_transaction();
  return result;
}

template<class DBConnector>
bool MessageDbHandler<DBConnector>::mark_read(IdType msg_id) {
  // Returns false on invalid id
  db_session_.start_transaction();
  MessagePtr msg = db_session_.template get_by_id<MessageDbData>(msg_id);
  bool result = (bool) msg;
  if (msg) {
    msg.modify()->is_read_ = true;
  }
  db_session_.end_transaction();
  return result;
}

template<class DBConnector>
void MessageDbHandler<DBConnector>::mark_read(MessagePtr &msg) {
  db_session_.start_transaction();
  msg.modify()->is_read_ = true;
  db_session_.end_transaction();
}

template<class DBConnector>
ContentPtr MessageDbHandler<DBConnector>::add_content(IdType msg_id, ContentDbData::Type type,
                                                      const std::string &path, const std::string &metadata) {
  MessagePtr message = db_session_.template get_by_id<MessageDbData>(msg_id);
  if (!message) {
    return nullptr;
  }
  return add_content(message, type, path, metadata);
}

template<class DBConnector>
ContentPtr MessageDbHandler<DBConnector>::add_content(MessagePtr &msg, ContentDbData::Type type,
                                                      const std::string &path, const std::string &metadata) {
  auto new_content = std::make_unique<ContentDbData>(ContentDbData());
  new_content->type_ = type;
  new_content->file_path_ = path;
  new_content->metadata_ = metadata;
  db_session_.start_transaction();
  new_content->message_ = msg;
  ContentPtr result = db_session_.add(std::move(new_content));
  db_session_.end_transaction();
  return result;
}

template<class DBConnector>
MessagePtr MessageDbHandler<DBConnector>::get_last_msg(IdType dialogue_id) {
  db_session_.start_transaction();
  MessagePtr result = db_session_.template find<MessageDbData>().where("dialogue_id = ?").bind(dialogue_id)
                                                                    .orderBy("creation_dt DESC").limit(1);
  db_session_.end_transaction();
  return result;
}
