#include <iostream>
#include <string>

#include <Wt/WServer.h>

#include "Auth.hpp"

#include "ChatServer.hpp"
#include "ChatApplication.hpp"

class TokenGenerator : public ITokenGenerator {
public:
    std::string generate_token() override { return std::string();};
};

std::unique_ptr<Wt::WApplication> create_application(const Wt::WEnvironment& env, ChatServer& server) {
    return std::make_unique<ChatApplication>(env, server);
}

int main(int argc, char **argv) {

    Wt::WServer server(argc, argv, WTHTTP_CONFIGURATION);
    ChatServer chatServer(server);

    // Adding entry-point for full-window app
    server.addEntryPoint(Wt::EntryPointType::Application,
                        std::bind(create_application, std::placeholders::_1, std::ref(chatServer)));


    return 0;
}
