#pragma once

#include <string>

class FakeDbConnector {
 public:
  bool connect(const std::string &db_params);
  void disconnect();
};



