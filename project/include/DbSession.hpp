#pragma once

#include <memory>
#include <Models.hpp>

namespace dbo = Wt::Dbo;

template< class DBConnector >
class DbSession {
 private:
  dbo::Session db_session_;
 public:
  DbSession(const std::string &host, uint32_t port, const std::string &user,
            const std::string &password, const std::string &db_name);
  DbSession(const DbSession &) = delete;
  ~DbSession();
  template<class C>
  dbo::ptr<C> add(std::unique_ptr<C> ptr);
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
  dbo::Transaction transaction(db_session_);
  auto result = db_session_.add(std::move(ptr));
  transaction.commit();
  return result;
}

