#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

template < class DBConnector >
class DialogueManager {
 public:
  DbSession<DBConnector> &db_session_;
  explicit DialogueManager(DbSession<DBConnector> &db_session);
  DialogueModelPtr get_dialogue(uint32_t member_a, uint32_t member_b);
};

template < class DBConnector >
DialogueManager<DBConnector>::DialogueManager(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template < class DBConnector >
DialogueModelPtr DialogueManager<DBConnector>::get_dialogue(uint32_t member_a, uint32_t member_b) {
  return nullptr;
}