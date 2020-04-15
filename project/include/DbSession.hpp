#pragma once

#include <memory>

template< class DBConnector >
class DbSession {
 public:
  std::unique_ptr<DBConnector> db_connector_;
  DbSession() = default;
  DbSession(const DbSession &) = delete;
  ~DbSession();
  explicit DbSession(std::unique_ptr<DBConnector> db_connector);
  bool connect(const std::string &host, uint32_t port, const std::string &user,
               const std::string &password, const std::string &db_name);
  void disconnect();
};

template< class DBConnector >
DbSession<DBConnector>::DbSession(std::unique_ptr<DBConnector> db_connector)
    : db_connector_(std::move(db_connector)) {}

template< class DBConnector >
bool DbSession<DBConnector>::connect(const std::string &host, uint32_t port,
                                     const std::string &user, const std::string &password,
                                     const std::string &db_name) {
  if (!db_connector_) {
    return false;
  }
  unsigned long len_limit = host.size() + user.size() + password.size() + db_name.size() + 64;
  auto buf = new char[len_limit];
  snprintf(buf, sizeof(buf), "host=%s user=%s password=%s port=%u dbname=%s",
           host.c_str(), user.c_str(), password.c_str(), port, db_name.c_str());
  bool result = db_connector_->connect(buf);
  delete[] buf;
  return result;
}

template< class DBConnector >
void DbSession<DBConnector>::disconnect() {
  if (db_connector_) {
    db_connector_->disconnect();
  }
}

template<class DBConnector>
DbSession<DBConnector>::~DbSession() {
  if (db_connector_) {
    db_connector_->disconnect();
  }
}


