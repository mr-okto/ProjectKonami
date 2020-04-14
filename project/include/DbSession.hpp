#pragma once

#include <memory>

template< class DBConnector >
class DbSession {
 private:
  std::unique_ptr<DBConnector> db_connector_;
 public:
  explicit DbSession(std::unique_ptr<DBConnector> db_connector);
  bool connect(const std::string &host, uint32_t port,
               const std::string &password, const std::string &db_name);
  void disconnect();
};

template< class DBConnector >
DbSession<DBConnector>::DbSession(std::unique_ptr<DBConnector> db_connector)
    : db_connector_(std::move(db_connector)) {}

template< class DBConnector >
bool DbSession<DBConnector>::connect(const std::string &host,
                                     uint32_t port,
                                     const std::string &password,
                                     const std::string &db_name) {
  return false;
}

template< class DBConnector >
void DbSession<DBConnector>::disconnect() {
  return;
}


