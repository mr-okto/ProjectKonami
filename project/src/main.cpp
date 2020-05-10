#include <iostream>
#include <string>

#include <Wt/WServer.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>

#include "Auth.hpp"

#include "ChatServer.hpp"
#include "ChatApplication.hpp"

std::unique_ptr<Wt::WApplication> create_application(const Wt::WEnvironment& env, ChatServer& server) {
    return std::make_unique<ChatApplication>(env, server);
}

int main(int argc, char **argv) {
    DbSession<Wt::Dbo::backend::Postgres> session;
    std::ifstream f_in;
    f_in.open("project/db_conf.json");
    session.connect(f_in);
    f_in.close();

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
