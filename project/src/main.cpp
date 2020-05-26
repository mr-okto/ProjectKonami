#include <iostream>
#include <string>
#include <filesystem>

#include <Wt/WServer.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>

#include "Auth.hpp"

#include "ChatServer.hpp"
#include "ChatApplication.hpp"

std::unique_ptr<Wt::WApplication> create_application(const Wt::WEnvironment& env, ChatServer& server) {
  return std::make_unique<ChatApplication>(env, server);
}

void create_folders_if_does_not_exist() {
  auto result = std::filesystem::create_directory("./media");
  std::filesystem::create_directory("./avatars");
  if (result) {
    std::filesystem::create_directory("./media/video");
    std::filesystem::create_directory("./media/image");
    std::filesystem::create_directory("./media/other");
  }
}

int main(int argc, char **argv) {
  create_folders_if_does_not_exist();
  DbSession<Wt::Dbo::backend::Postgres> session;
  auto fname = DbSession<Wt::Dbo::backend::Postgres>::dbconfig_filename(argc, argv);
  if (!session.connect(fname)) {
    return 1;
  }

  Wt::WServer server(argc, argv, WTHTTP_CONFIGURATION);
  ChatServer chatServer(server, session);

  // Adding entry-point for full-window app
  server.addEntryPoint(Wt::EntryPointType::Application,
                       std::bind(create_application, std::placeholders::_1, std::ref(chatServer)));

  if (server.start()) {
    int sig = Wt::WServer::waitForShutdown();
    std::cerr << "Shutting down: (signal = " << sig << ")" << std::endl;
    server.stop();
    session.disconnect();
  }

  return 0;
}
