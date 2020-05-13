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
  MessageModelPtr get_last_msg(IdType dialogue_id);
  bool mark_read(IdType msg_id);
  void mark_read(MessageModelPtr &msg);
  ContentModelPtr add_content(IdType msg_id, ContentModel::Type type, const std::string &path,
                              const std::string &metadata = {});
  ContentModelPtr add_content(MessageModelPtr &msg, ContentModel::Type type, const std::string &path,
                              const std::string &metadata = {});
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
  db_session_.end_transaction();
  return result;
}

template < class DBConnector >
Messages MessageManager<DBConnector>::get_latest_messages(IdType dialogue_id, time_t start) {
  db_session_.start_transaction();
  Messages result = db_session_.template find<MessageModel>().where("dialogue_id = ?").bind(dialogue_id)
                                                             .where("creation_dt >= ?").bind(start)
                                                             .orderBy("creation_dt");
  db_session_.end_transaction();
  return result;
}

template<class DBConnector>
bool MessageManager<DBConnector>::mark_read(IdType msg_id) {
  // Returns false on invalid id
  db_session_.start_transaction();
  MessageModelPtr msg = db_session_.template get_by_id<MessageModel>(msg_id);
  bool result = (bool) msg;
  if (msg) {
    msg.modify()->is_read_ = true;
  }
  db_session_.end_transaction();
  return result;
}

template<class DBConnector>
void MessageManager<DBConnector>::mark_read(MessageModelPtr &msg) {
  db_session_.start_transaction();
  msg.modify()->is_read_ = true;
  db_session_.end_transaction();
}

template<class DBConnector>
ContentModelPtr MessageManager<DBConnector>::add_content(IdType msg_id, ContentModel::Type type,
                                                         const std::string &path, const std::string &metadata) {
  MessageModelPtr message = db_session_.template get_by_id<MessageModel>(msg_id);
  if (!message) {
    return nullptr;
  }
  return add_content(message, type, path, metadata);
}

template<class DBConnector>
ContentModelPtr MessageManager<DBConnector>::add_content(MessageModelPtr &msg, ContentModel::Type type,
                                                         const std::string &path, const std::string &metadata) {
  auto new_content = std::make_unique<ContentModel>(ContentModel());
  new_content->type_ = type;
  new_content->file_path_ = path;
  new_content->metadata_ = metadata;
  db_session_.start_transaction();
  new_content->message_ = msg;
  ContentModelPtr result = db_session_.add(std::move(new_content));
  db_session_.end_transaction();
  return result;
}

template<class DBConnector>
MessageModelPtr MessageManager<DBConnector>::get_last_msg(IdType dialogue_id) {
  db_session_.start_transaction();
  MessageModelPtr result = db_session_.template find<MessageModel>().where("dialogue_id = ?").bind(dialogue_id)
                                                                    .orderBy("creation_dt DESC").limit(1);
  db_session_.end_transaction();
  return MessageModelPtr();
}
