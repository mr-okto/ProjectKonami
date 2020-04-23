#include <Wt/WServer.h>

#include "ChatServer.hpp"

class UserManagerStub : public UserManager {
public:
    UserManagerStub() : stubUsers() {}

    UserModelPtr get_user(const std::string &username) override {
        stubUser.username_ = "login";
        stubUser.pwd_hash_ = "pass";
        stubUser.id_ = 0;

        return &stubUser;
    }
    UserModelPtr create_user(const std::string &username, const std::string &pwd_hash) {

        if (stubUsers.find(username) != stubUsers.end()) {
            return nullptr;
        }

        UserModel um;
        um.username_ = username;
        um.pwd_hash_ = pwd_hash;
        um.id_ = stubUsers.size();

        stubUsers[username] = um;

        return &stubUsers[username];
    }


    UserModel stubUser;
    std::map<std::string , UserModel> stubUsers;
};

UserManagerStub userManagerStub;

ChatServer::ChatServer(Wt::WServer& server)
    : server_(server),
      sessionManager_(),
      authService_(userManagerStub, &sessionManager_)
{

}

bool ChatServer::sign_in(const Wt::WString& username, const Wt::WString& password) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    AuthData data;
    if (authService_.sign_in(username.toUTF8(), password.toUTF8(), std::ref(data))) {
        Session s(data.id);
        sessionManager_.add_session(s);

        return true;
    } else
        return false;

}

bool ChatServer::connect(Client *client, const ChatEventCallback &handle_event) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

//    if (sessionManager_.has(client)) {
//        return false;
//    }


}

bool ChatServer::sign_up(const Wt::WString& username, const Wt::WString& password) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    if (authService_.sign_up(username.toUTF8(), password.toUTF8())) {
        return true;
    } else
        return false;
}

