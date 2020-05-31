#include "DbConnectorStub.hpp"

bool DbConnectorStub::connect(const std::string &db_params) {
  return false;
}

void DbConnectorStub::disconnect() {
}

bool DbConnectorStub::truncate_tables() {
  return false;
}
