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
  DialogueModelPtr result = nullptr;
  db_session_.start_transaction();
  IdType dialogue_id = db_session_.template raw_query<IdType>("select t1.dialogue_id from dialogue_members t1")
      .join("dialogue_members t2 on t1.dialogue_id = t2.dialogue_id and t2.user_id = ?")
      .bind(member_b).where("t1.user_id = ?").bind(member_a);
  if (dialogue_id) {
    result = db_session_.template get_by_id<DialogueModel>(dialogue_id);
  }
  if (not result) {
    auto new_dialogue = std::make_unique<DialogueModel>(DialogueModel());
    result = db_session_.add(std::move(new_dialogue));
    UserModelPtr member_ptr = db_session_.template get_by_id<UserModel>(member_a);
    result.modify()->members_.insert(member_ptr);
    member_ptr = db_session_.template get_by_id<UserModel>(member_b);
    result.modify()->members_.insert(member_ptr);
  }
  db_session_.end_transaction();
  return result;
}
