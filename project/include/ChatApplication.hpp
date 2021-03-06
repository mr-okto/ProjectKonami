#ifndef PROJECTKONAMI_CHATAPPLICATION_HPP
#define PROJECTKONAMI_CHATAPPLICATION_HPP

#include <Wt/WApplication.h>

#include <ChatServer.hpp>

class ChatApplication : public Wt::WApplication {
public:
    ChatApplication(const Wt::WEnvironment& env, ChatServer& server);

protected:
    virtual void idleTimeout() override;

private:
    ChatServer& server_;
    const Wt::WEnvironment& env_;

    bool logged_in_;
    void start_chat(const Wt::WString& username, uint32_t id, const std::optional<std::string>& cookie);

    void start_auth();

    Wt::Core::observing_ptr<Wt::WDialog> idle_timeout_dialog_;
};


#endif //PROJECTKONAMI_CHATAPPLICATION_HPP
