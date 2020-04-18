#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

template < class DBConnector >
class MessageManager {
 public:
  DbSession<DBConnector> &db_session_;
  explicit MessageManager(DbSession<DBConnector> &db_session);
  MessageModelPtr create_msg(uint32_t dialogue_id,
                             uint32_t author_id, const std::string &text);
  std::vector<MessageModelPtr> get_latest_messages(uint32_t dialogue_id, time_t start);
};

template < class DBConnector >
MessageManager<DBConnector>::MessageManager(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template < class DBConnector >
MessageModelPtr MessageManager<DBConnector>::create_msg(uint32_t dialogue_id, uint32_t author_id,
                                                        const std::string &text) {
  return nullptr;
}

template < class DBConnector >
std::vector<MessageModelPtr> MessageManager<DBConnector>::get_latest_messages(uint32_t dialogue_id, time_t start) {
  return std::vector<MessageModelPtr>();
}