#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

class MessageManager {
 public:
  DbSession &db_session_;
  explicit MessageManager(DbSession &db_session);
  MessageModelPtr create_msg(uint32_t dialogue_id,
                             uint32_t author_id, const std::string &text);
  std::vector<MessageModelPtr> get_latest_messages(uint32_t dialogue_id, time_t start);
};
