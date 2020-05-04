#include <Wt/WServer.h>

#include "ChatServer.hpp"

class UserManagerStub : public UserManager {
public:
    UserManagerStub() : stubUsers()
    {
        UserModel um;
        um.username_ = "login";
        um.pwd_hash_ = "pass";
        um.id_ = 0;
        stubUsers.insert(std::pair<std::string, UserModel>("login", um));
    }

    UserModelPtr get_user(const std::string &username) override {
        if (stubUsers.find(username) == stubUsers.end()) {
            return nullptr;
        }

        return &stubUsers[username];
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
      authService_(userManagerStub, &sessionManager_),
      dialogue_service_()
{
}

bool ChatServer::sign_in(const Wt::WString& username, const Wt::WString& password) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    AuthData data;
    if (authService_.sign_in(username.toUTF8(), password.toUTF8(), std::ref(data))) {
        online_users_.insert(username);

        post_chat_event(ChatEvent(ChatEvent::SIGN_IN, username,
                                  sessionManager_.user_id(username.toUTF8())));

        return true;
    } else
        return false;

}

bool ChatServer::connect(Client *client, const ChatEventCallback &handle_event) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    std::string username = client->username();
    if (sessionManager_.has_reserved(username)) {
        Session s;
        s.username_ = username;
        s.user_id_ = sessionManager_.user_id(username);
        s.session_id_ = Wt::WApplication::instance()->sessionId();
        s.time_point_ = std::chrono::system_clock::now();
        s.status_ = Session::Status::Active;
        s.callback_ = handle_event;

        return sessionManager_.add_session(client, s);
    } else
        return false;

}

bool ChatServer::sign_up(const Wt::WString& username, const Wt::WString& password) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    if (authService_.sign_up(username.toUTF8(), password.toUTF8())) {
        return true;
    } else
        return false;
}

bool ChatServer::sign_out(const Wt::WString &username_) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    std::string username = username_.toUTF8();
    if (sessionManager_.has_reserved(username)) {
        sessionManager_.unreserve(username);
        return true;
    } else
        return false;
}

bool ChatServer::disconnect(Client *client) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    return sessionManager_.close_session(client);
}

void ChatServer::post_chat_event(const ChatEvent& event) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    Wt::WApplication *app = Wt::WApplication::instance();

    auto clients = sessionManager_.active_sessions();
    for (auto iter = clients.begin(); iter != clients.end(); ++iter) {
        if (app && app->sessionId() == iter->second.session_id_) {
            iter->second.callback_(event);
        } else {
            server_.post(iter->second.session_id_,
                            std::bind(iter->second.callback_, event));
        }

    }
}

std::set<Wt::WString> ChatServer::online_users() {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    return online_users_;
}

std::vector<chat::Dialogue> ChatServer::get_dialogues(const Wt::WString& username) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    std::vector<chat::Dialogue> vec = {{1, "bob"},
                                       {2, "lel"}, 
                                       {3, "kek"}};
    return vec;
    /*uint user_id = sessionManager_.user_id(username.toUTF8());

    return dialogue_service_.get_dialogues(user_id);*/
}

std::vector<chat::Message> ChatServer::get_messages(uint dialogue_id) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    if (dialogue_id == 1) {
        std::vector<chat::Message> vec = {{dialogue_id, "bob", "first msg", time(NULL)}};
        return vec;
    } else if (dialogue_id == 2) {
        std::vector<chat::Message> vec = {{dialogue_id, "lel", "seconde msg", time(NULL)}};
        return vec;
    } else {
        std::vector<chat::Message> vec = {{dialogue_id, "kek", "third msg", time(NULL)}};
        return vec;
    }

    //return dialogue_service_.get_messages(dialogue_id);
}

chat::Message ChatServer::send_msg(const Wt::WString& username, 
                                   uint dialogue_id, 
                                   const Wt::WString& message) {
                    
    /*std::unique_lock<std::recursive_mutex> lock(mutex_);
    uint user_id = sessionManager_.user_id(username.toUTF8());*/
    chat::Message new_message = {dialogue_id, username.toUTF8(), message.toUTF8(), time(NULL)};
    return new_message;
    //return dialogue_service_.post_message(new_message);
}

uint ChatServer::get_user_id(const Wt::WString& username) {
    return sessionManager_.user_id(username.toUTF8());
}
