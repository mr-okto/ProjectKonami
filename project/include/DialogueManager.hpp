#pragma once

#include "DbSession.hpp"
#include "Models.hpp"

template < class DBConnector >
class DialogueManager {
 public:
  DbSession<DBConnector> &db_session_;
  explicit DialogueManager(DbSession<DBConnector> &db_session);
  DialogueModelPtr get_dialogue(IdType member_a, IdType member_b);
  std::tuple<DialogueModelPtr, bool> get_or_create_dialogue(IdType member_a, IdType member_b);
  std::tuple<DialogueModelPtr, bool> get_or_create_dialogue(const UserModelPtr &member_a, const UserModelPtr &member_b);
};

template < class DBConnector >
DialogueManager<DBConnector>::DialogueManager(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template < class DBConnector >
DialogueModelPtr DialogueManager<DBConnector>::get_dialogue(IdType member_a, IdType member_b) {
  // Returns nullptr if dialogue does not exist
  DialogueModelPtr result = nullptr;
  db_session_.start_transaction();
  IdType dialogue_id = db_session_.template raw_query<IdType>("select t1.dialogue_id from dialogue_members t1")
      .join("dialogue_members t2 on t1.dialogue_id = t2.dialogue_id and t2.user_id = ?")
      .bind(member_b).where("t1.user_id = ?").bind(member_a);
  if (dialogue_id) {
    result = db_session_.template get_by_id<DialogueModel>(dialogue_id);
  }
  db_session_.end_transaction();
  return result;
}

template < class DBConnector >
std::tuple<DialogueModelPtr, bool> DialogueManager<DBConnector>::get_or_create_dialogue(const UserModelPtr &member_a,
                                                                                        const UserModelPtr &member_b) {
  DialogueModelPtr result = get_dialogue(member_a.id(), member_b.id());
  bool is_new = (result)? false : true;
  if (not result) {
    auto new_dialogue = std::make_unique<DialogueModel>(DialogueModel());
    result = db_session_.add(std::move(new_dialogue));
    result.modify()->members_.insert(member_a);
    result.modify()->members_.insert(member_b);
  }
  db_session_.end_transaction();
  return std::tuple(result, is_new);
}


template < class DBConnector >
std::tuple<DialogueModelPtr, bool> DialogueManager<DBConnector>::get_or_create_dialogue(IdType member_a,
                                                                                        IdType member_b) {
  UserModelPtr member_a_ptr = db_session_.template get_by_id<UserModel>(member_a);
  UserModelPtr member_b_ptr = db_session_.template get_by_id<UserModel>(member_b);
  if (member_a_ptr && member_b_ptr) {
    return get_or_create_dialogue(member_a_ptr, member_b_ptr);
  }
  return std::tuple(nullptr, false);
}
