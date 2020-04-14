#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

class Dialogue_Manager {
 public:
  DbSession &db_session_;
  explicit Dialogue_Manager(DbSession &db_session);
  DialogueModelPtr get_dialogue(uint32_t member_a, uint32_t member_b);
};
