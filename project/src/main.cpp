#include <iostream>
#include "Models.hpp"
#include "DialogueManager.hpp"
#include "MessageManager.hpp"
#include "UserManager.hpp"
#include "DbSession.hpp"

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>


int main(int argc, char **argv) {
  DbSession<Wt::Dbo::backend::Postgres> session;
  std::ifstream f_in;
  auto fname = DbSession<Wt::Dbo::backend::Postgres>::dbconfig_filename(argc, argv);
  if (!session.connect(fname)) {
    return 1;
  }

  UserManager user_manager(session);
  DialogueManager dialogue_manager(session);
  MessageManager message_manager(session);

  auto time = std::time(nullptr);
  std::stringstream buffer;
  buffer << std::put_time(std::gmtime(&time), "%F %T%z") << std::endl;
  MessageModelPtr msg = message_manager.create_msg(1, 1, buffer.str());
  if (msg) {
    std::cout << msg->text_;
  } else {
    std::cout << "Empty";
  }
  session.disconnect();
  return 0;
}