#pragma once
#include <Wt/WServer.h>
#include <Wt/WString.h>
#include "DialogueService.hpp"
#include "UserService.hpp"

namespace chat {

class AuthSFA {};

class ChatServer {
 public:
    ChatServer(Wt::WServer& server, AuthSFA& auth);
    Wt::WString sing_in(const Wt::WString& login, const Wt::WString& password);
    Wt::WString sing_up(const Wt::WString& login, const Wt::WString& password);
    bool sing_out(const Wt::WString& token);

 private:
    Wt::WServer& server_;
    DialogueService dialogue_service_;
    UserService user_service_;
    AuthSFA auth_;
    std::recursive_mutex mutex_;
};
}