#include "FakeDbConnector.hpp"

bool FakeDbConnector::connect(const std::string &db_params) {
  return false;
}

void FakeDbConnector::disconnect() {
}
