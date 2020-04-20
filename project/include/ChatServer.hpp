#ifndef PROJECTKONAMI_CHATSERVER_HPP
#define PROJECTKONAMI_CHATSERVER_HPP

#include <Wt/WServer.h>
#include <mutex>

#include "Auth.hpp"
#include "SessionManager.hpp"
#include "ChatEvent.hpp"

// a reference to client
class Client {
public:
    virtual void connect() = 0;
    virtual void disconnect() = 0;

    virtual void sign_in() = 0;
    virtual void sign_out() = 0;

    virtual void sign_up() = 0;
};

class ChatServer : public Wt::WServer {
public:
    ChatServer(Wt::WServer& server);

    // delete
    ChatServer(const ChatServer&) = delete;
    ChatServer& operator=(const ChatServer&) = delete;

    // The passed callback method is posted to when a new chat event is received
    // Return false if the client was already connected
    bool connect(Client *client, const ChatEventCallback& handle_event);
    bool disconnect(Client *client);

    // Try to sign in with given username and password.
    // Returns false if the login was not successful;
    bool sign_in(const Wt::WString& username, const Wt::WString& password);

private:
    Wt::WServer& server_;
    std::recursive_mutex mutex_;

    SessionManager sessionManager_;
    Auth           authService_;

};


#endif //PROJECTKONAMI_CHATSERVER_HPP
