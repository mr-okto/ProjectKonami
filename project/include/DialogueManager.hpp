#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

template < class DBConnector >
class DialogueManager {
 public:
  DbSession<DBConnector> &db_session_;
  explicit DialogueManager(DbSession<DBConnector> &db_session);
  DialogueModelPtr get_dialogue(IdType member_a, IdType member_b);
};

template < class DBConnector >
DialogueManager<DBConnector>::DialogueManager(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template < class DBConnector >
DialogueModelPtr DialogueManager<DBConnector>::get_dialogue(IdType member_a, IdType member_b) {
  auto new_dialogue = std::make_unique<DialogueModel>(DialogueModel());
  db_session_.start_transaction();
  UserModelPtr member_ptr = db_session_.template get_by_id<UserModel>(member_a);
  new_dialogue->members_.insert(member_ptr);
  member_ptr = db_session_.template get_by_id<UserModel>(member_b);
  new_dialogue->members_.insert(member_ptr);
  DialogueModelPtr result = db_session_.add(std::move(new_dialogue));
  db_session_.commit_transaction();
  return result;
}
