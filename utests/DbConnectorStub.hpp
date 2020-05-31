#pragma once

#include <string>

class DbConnectorStub {
 public:
  bool connect(const std::string &db_params);
  bool truncate_tables();
  void disconnect();
};



