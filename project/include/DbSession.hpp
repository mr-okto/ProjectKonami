#pragma once

#include <memory>
#include <Models.hpp>

namespace dbo = Wt::Dbo;

template< class DBConnector >
class DbSession {
 private:
  dbo::Session db_session_;
  std::unique_ptr<dbo::Transaction> transaction_;
 public:
  DbSession(const std::string &host, uint32_t port, const std::string &user,
            const std::string &password, const std::string &db_name);
  DbSession(const DbSession &) = delete;
  ~DbSession();
  void start_transaction();
  bool end_transaction();
  template<class C>
  dbo::ptr<C> add(std::unique_ptr<C> ptr);
  template<class C>
  dbo::ptr<C> get_by_id(long long int id);
  template<class C>
  dbo::Query<dbo::ptr<C>> find();
  template<class C>
  dbo::Query<C> raw_query(const std::string & sql);
};

template<class DBConnector>
DbSession<DBConnector>::~DbSession() {
    db_session_.flush();
}

template<class DBConnector>
DbSession<DBConnector>::DbSession(const std::string &host,
                                  uint32_t port,
                                  const std::string &user,
                                  const std::string &password,
                                  const std::string &db_name) {
  unsigned long len_limit = host.size() + user.size() + password.size() + db_name.size() + 64;
  auto buf = new char[len_limit];
  snprintf(buf, len_limit, "host=%s user=%s password=%s port=%u dbname=%s",
           host.c_str(), user.c_str(), password.c_str(), port, db_name.c_str());
  std::unique_ptr<DBConnector> db_connector{new DBConnector()};
  db_connector->connect(buf);
  delete[] buf;
  db_session_.setConnection(std::move(db_connector));
  db_session_.mapClass<DialogueModel>("dialogue");
  db_session_.mapClass<MessageModel>("message");
  db_session_.mapClass<UserModel>("user");
  db_session_.mapClass<PictureModel>("picture");
}

template<class DBConnector>
template<class C>
dbo::ptr<C> DbSession<DBConnector>::add(std::unique_ptr<C> ptr) {
  if (not transaction_) {
    // Disposable transaction
    dbo::Transaction transaction(db_session_);
    return transaction.session().add(std::move(ptr));
  }
  auto result = db_session_.add(std::move(ptr));
  return result;
}

template<class DBConnector>
template<class C>
dbo::ptr<C> DbSession<DBConnector>::get_by_id(IdType id) {
  if (not transaction_) {
    // Disposable transaction
    dbo::Transaction transaction(db_session_);
    return transaction.session().find<C>().where("id = ?").bind(id);
  }
  return db_session_.find<C>().where("id = ?").bind(id);
}

template<class DBConnector>
void DbSession<DBConnector>::start_transaction() {
  if (not transaction_) {
    transaction_ = std::make_unique<dbo::Transaction>(db_session_);
  }
}

template<class DBConnector>
bool DbSession<DBConnector>::end_transaction() {
  if (transaction_) {
    transaction_->commit();
    transaction_.reset();
    return true;
  }
  return false;
}

template<class DBConnector>
template<class C>
dbo::Query<dbo::ptr<C>>  DbSession<DBConnector>::find() {
  if (not transaction_) {
    dbo::Transaction transaction(db_session_);
    return transaction.session().find<C>();
  }
  return db_session_.find<C>();
}

template<class DBConnector>
template<class C>
dbo::Query<C> DbSession<DBConnector>::raw_query(const std::string &sql) {
  if (not transaction_) {
    dbo::Transaction transaction(db_session_);
    return transaction.session().query<C>(sql);
  }
  return db_session_.query<C>(sql);
}

