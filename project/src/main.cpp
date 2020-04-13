#include <Wt/WServer.h>
#include "ChatServer.hpp"
#include "ChatApplication.hpp"

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env,
				ChatServer& server)
{
  std::cout << "bolshol lel" << std::endl;
  std::cout << "big baobab" << std::endl;
  return Wt::cpp14::make_unique<ChatApplication>(env, server);
}


int main(int argc, char **argv)
{
    Wt::WServer server(argc, argv, WTHTTP_CONFIGURATION);
    // WTHTTP_CONFIGURATION is e.g. "/etc/wt/wthttpd"
    // add a single entry point, at the default location (as determined
    // by the server configuration's deploy-path)
    ChatServer chat_server(server);
    server.addEntryPoint(Wt::EntryPointType::Application,
                       std::bind(createApplication, std::placeholders::_1,
                                   std::ref(chat_server)));
    if (server.start()) {
      int sig = Wt::WServer::waitForShutdown();
      std::cerr << "Shutdown (signal = " << sig << ")" << std::endl;
      server.stop();
    }
}