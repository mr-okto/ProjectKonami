#include "MessageManager.hpp"

MessageManager::MessageManager(DbSession &db_session)
    : db_session_(db_session) {}

MessageModelPtr MessageManager::create_msg(uint32_t dialogue_id, uint32_t author_id, const std::string &text) {
  return nullptr;
}

std::vector<MessageModelPtr> MessageManager::get_latest_messages(uint32_t dialogue_id, time_t start) {
  return std::vector<MessageModelPtr>();
}
