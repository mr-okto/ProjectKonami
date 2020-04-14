#include "DbSession.hpp"

DbSession::DbSession(std::unique_ptr<Postgres> db_connector)
  : db_connector_(std::move(db_connector)) {}

bool DbSession::connect(const std::string &host, uint32_t port,
                        const std::string &password, const std::string &db_name) {
  return false;
}
void DbSession::disconnect() {
  return;
}

