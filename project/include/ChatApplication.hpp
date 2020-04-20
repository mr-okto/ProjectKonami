#ifndef PROJECTKONAMI_CHATAPPLICATION_HPP
#define PROJECTKONAMI_CHATAPPLICATION_HPP

#include <Wt/WApplication.h>

#include <ChatServer.hpp>

class ChatApplication : public Wt::WApplication {
public:
    ChatApplication(const Wt::WEnvironment& env, ChatServer& server);

private:
    ChatServer& server_;
    const Wt::WEnvironment& env_;



};


#endif //PROJECTKONAMI_CHATAPPLICATION_HPP