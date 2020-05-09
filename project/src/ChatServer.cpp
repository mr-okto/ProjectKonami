#include <Wt/WServer.h>
#include <Wt/WEnvironment.h>

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

std::optional<uint32_t> ChatServer::sign_in(const Wt::WString& username, const Wt::WString& password) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    uint32_t id = 0;
    if (authService_.sign_in(username.toUTF8(), password.toUTF8(), &id)) {
        online_users_.insert(username);

        post_chat_event(ChatEvent(ChatEvent::SIGN_IN, username, id));

        return id;
    } else
        return std::nullopt;

}

bool ChatServer::connect(Client *client, const ChatEventCallback &handle_event) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    std::cout << "ACTIVE SESSIONS : " << sessionManager_.active_sessions().size() << std::endl;
    std::cout << "ONLINE USERS : " << online_users_.size() << std::endl;

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
    sessionManager_.unreserve(username);
    online_users_.erase(username_);
    std::cout << "SIGN OUT"<< std::endl;

    post_chat_event(ChatEvent(ChatEvent::Type::SIGN_OUT, username));

    return true;
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

void ChatServer::notify_user(const ChatEvent& event) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    auto clients = sessionManager_.active_sessions();
    for (auto iter = clients.begin(); iter != clients.end(); ++iter) {
        if (iter->second.username_ == event.username_.toUTF8()) {
            auto callback = iter->second.callback_;
            auto session_id = iter->second.session_id_;
            server_.post(session_id, std::bind(callback, event));
            return;
        }
    }
}

std::set<Wt::WString> ChatServer::online_users() {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    return online_users_;
}

std::vector<chat::Dialogue> ChatServer::get_dialogues(const Wt::WString& username) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    return dialogue_service_.get_dialogues(username.toUTF8());
}

std::vector<chat::Message> ChatServer::get_messages(uint dialogue_id) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    return dialogue_service_.get_messages(dialogue_id);
}

chat::Message ChatServer::send_msg(uint dialogue_id, 
                                   const Wt::WString& username,
                                   const Wt::WString& receiver_name,
                                   const Wt::WString& message_content) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    chat::Message message = dialogue_service_.post_message(dialogue_id, username.toUTF8(), message_content.toUTF8());
    if (online_users_.count(username)) {
        notify_user(ChatEvent(dialogue_id, receiver_name));
    }         
    return message;
}

bool ChatServer::create_dialogue(const Wt::WString& creater, const Wt::WString& receiver) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    if (dialogue_service_.create_dialogue(creater.toUTF8(), receiver.toUTF8())) {
        notify_user(ChatEvent(ChatEvent::NEW_DIALOGUE, receiver));
        return true;
    }
    return false;
}

uint ChatServer::get_user_id(const Wt::WString& username) {
    return sessionManager_.user_id(username.toUTF8());
}

void ChatServer::set_cookie(const std::string& username, const std::string& cookie) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    online_users_.insert(username);
    sessionManager_.set_cookie(username, cookie);

    post_chat_event(ChatEvent(ChatEvent::SIGN_IN, username));
}

std::string ChatServer::check_cookie(const std::string& cookie) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    for (auto& session : sessionManager_.active_sessions()) {
        if (session.second.cookie_ == cookie) {
            return session.second.username_;
        }
    }
    return std::string{};
}

void ChatServer::weak_sign_out(Client *client , const Wt::WString& username_) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    std::string username = username_.toUTF8();
    //(FIXME) maybe delete 'has_reserved'
//    if (sessionManager_.has_reserved(username)) {
//        auto count = std::count_if(
//                sessionManager_.active_sessions().begin(), sessionManager_.active_sessions().end(),
//                [&username](auto& s) {
//                    return s.second.username_ == username;
//                });
//        std::cout << "COUNT: " << count << std::endl;
        // if there was no 'close_same_session' call
        if (sessionManager_.active_sessions().find(client) != sessionManager_.active_sessions().end()) {
            std::cout << "WEAK SIGN OUT"<< std::endl;
            online_users_.erase(username_);
            post_chat_event(ChatEvent(ChatEvent::Type::SIGN_OUT, username));
        }
//    }
}

void ChatServer::close_same_session(const Wt::WString& username_) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    std::string username = username_.toUTF8();
    for (auto& session : sessionManager_.active_sessions()) {
        if (session.second.username_ == username) {
            sessionManager_.close_session(session.first);
            return;
        }
    }
}
