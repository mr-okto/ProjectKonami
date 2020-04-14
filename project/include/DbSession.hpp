#pragma once

#include <memory>

#define Postgres int

class DbSession {
 private:
  std::unique_ptr<Postgres> db_connector_;
 public:
  explicit DbSession(std::unique_ptr<Postgres> db_connector);
  bool connect(const std::string &host, uint32_t port,
               const std::string &password, const std::string &db_name);
  void disconnect();
};

