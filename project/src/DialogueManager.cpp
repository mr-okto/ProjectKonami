#include "DialogueManager.hpp"

Dialogue_Manager::Dialogue_Manager(DbSession &db_session)
    : db_session_(db_session) {}

DialogueModelPtr Dialogue_Manager::get_dialogue(uint32_t member_a, uint32_t member_b) {
  return nullptr;
}
