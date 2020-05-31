#pragma once

#include "DbSession.hpp"
#include "DbData.hpp"

template < class DBConnector >
class DialogueDbHandler {
 public:
  DbSession<DBConnector> &db_session_;
  explicit DialogueDbHandler(DbSession<DBConnector> &db_session);
  DialoguePtr get_dialogue(IdType member_a, IdType member_b);
  std::tuple<DialoguePtr, bool> get_or_create_dialogue(IdType member_a, IdType member_b);
  std::tuple<DialoguePtr, bool> get_or_create_dialogue(const UserPtr &member_a, const UserPtr &member_b);
  unsigned long count_messages(const UserPtr &sender, const UserPtr &recipient);
  unsigned long count_messages(IdType dialogue, IdType sender);
  unsigned long count_unread_messages(IdType dialogue, IdType recipient);
};

template < class DBConnector >
DialogueDbHandler<DBConnector>::DialogueDbHandler(DbSession<DBConnector> &db_session)
    : db_session_(db_session) {}

template < class DBConnector >
DialoguePtr DialogueDbHandler<DBConnector>::get_dialogue(IdType member_a, IdType member_b) {
  // Returns nullptr if dialogue does not exist
  DialoguePtr result = nullptr;
  db_session_.start_transaction();
  IdType dialogue_id = db_session_.template raw_query<IdType>("select t1.dialogue_id from dialogue_members t1")
      .join("dialogue_members t2 on t1.dialogue_id = t2.dialogue_id and t2.user_id = ?")
      .bind(member_b).where("t1.user_id = ?").bind(member_a);
  if (dialogue_id) {
    result = db_session_.template get_by_id<DialogueDbData>(dialogue_id);
  }
  db_session_.end_transaction();
  return result;
}

template < class DBConnector >
std::tuple<DialoguePtr, bool> DialogueDbHandler<DBConnector>::get_or_create_dialogue(const UserPtr &member_a,
                                                                                     const UserPtr &member_b) {
  db_session_.start_transaction();
  DialoguePtr result = get_dialogue(member_a.id(), member_b.id());
  bool is_new = (result)? false : true;
  if (not result) {
    auto new_dialogue = std::make_unique<DialogueDbData>(DialogueDbData());
    result = db_session_.add(std::move(new_dialogue));
    result.modify()->members_.insert(member_a);
    result.modify()->members_.insert(member_b);
  }
  db_session_.end_transaction();
  return std::tuple(result, is_new);
}


template < class DBConnector >
std::tuple<DialoguePtr, bool> DialogueDbHandler<DBConnector>::get_or_create_dialogue(IdType member_a,
                                                                                     IdType member_b) {
  UserPtr member_a_ptr = db_session_.template get_by_id<UserDbData>(member_a);
  UserPtr member_b_ptr = db_session_.template get_by_id<UserDbData>(member_b);
  if (member_a_ptr && member_b_ptr) {
    return get_or_create_dialogue(member_a_ptr, member_b_ptr);
  }
  return std::tuple(nullptr, false);
}

template<class DBConnector>
unsigned long DialogueDbHandler<DBConnector>::count_messages(const UserPtr &sender, const UserPtr &recipient) {
  db_session_.start_transaction();
  DialoguePtr dialogue = get_dialogue(sender.id(), recipient.id());
  db_session_.end_transaction();
  return count_messages(dialogue.id(), sender.id());
}

template<class DBConnector>
unsigned long DialogueDbHandler<DBConnector>::count_messages(IdType dialogue, IdType sender) {
  db_session_.start_transaction();
  Messages messages = db_session_.template find<MessageDbData>().where("dialogue_id = ?").bind(dialogue)
      .where("author_id = ?").bind(sender);
  unsigned long result = messages.size();
  db_session_.end_transaction();
  return result;
}

template<class DBConnector>
unsigned long DialogueDbHandler<DBConnector>::count_unread_messages(IdType dialogue, IdType sender) {
  db_session_.start_transaction();
  Messages messages = db_session_.template find<MessageDbData>().where("dialogue_id = ?").bind(dialogue)
      .where("author_id != ?").bind(sender).where("is_read = ?").bind(false);
  unsigned long result = messages.size();
  db_session_.end_transaction();
  return result;
}
