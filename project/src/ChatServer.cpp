#include <Wt/WServer.h>
#include <Wt/WEnvironment.h>

#include "ChatServer.hpp"
#include "ImageProcessing.hpp"

ChatServer::ChatServer(Wt::WServer& server, DbSession<Wt::Dbo::backend::Postgres>& session)
    : server_(server),
      db_session_(session),
      dialogue_service_(db_session_),
      auth_service_(user_manager_, &session_manager_),
      user_manager_(db_session_)
{
}

std::optional<uint32_t> ChatServer::sign_in(const Wt::WString& username, const Wt::WString& password) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    uint32_t id = 0;
    if (auth_service_.sign_in(username.toUTF8(), password.toUTF8(), &id)) {
        online_users_.insert(username);

        post_chat_event(ChatEvent(ChatEvent::SIGN_IN, username, id));

        return id;
    } else
        return std::nullopt;

}

bool ChatServer::connect(Client *client, const ChatEventCallback &handle_event) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    std::cout << "ACTIVE SESSIONS : " << session_manager_.active_sessions().size() << std::endl;
    std::cout << "ONLINE USERS : " << online_users_.size() << std::endl;

    std::string username = client->username();
    if (session_manager_.has_reserved(username)) {
        Session s;
        s.username_ = username;
        s.user_id_ = session_manager_.user_id(username);
        s.session_id_ = Wt::WApplication::instance()->sessionId();
        s.time_point_ = std::chrono::system_clock::now();
        s.status_ = Session::Status::Active;
        s.callback_ = handle_event;

        return session_manager_.add_session(client, s);
    } else
        return false;

}

bool ChatServer::sign_up
(const Wt::WString& username, const Wt::WString& password, const std::string& avatar_path) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    auto userModel = user_manager_.create_user(username.toUTF8(), password.toUTF8());
    if (userModel) {
        if (!avatar_path.empty()) {
            auto blurredPaths = create_blurred_copies(avatar_path, "./avatars", userModel.id(), 5);
            for (int i = 0; i < 5; ++i) {
                std::cout << blurredPaths[i] << std::endl;
                user_manager_.add_picture(userModel.id(), blurredPaths[i], i + 1);
            }
        }
        return true;
    } else {
        return false;
    }
}

bool ChatServer::sign_out(const Wt::WString &username_) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    std::string username = username_.toUTF8();
    session_manager_.unreserve(username);
    online_users_.erase(username_);
    avatar_map_.erase(username_);
    std::cout << "SIGN OUT"<< std::endl;

    post_chat_event(ChatEvent(ChatEvent::Type::SIGN_OUT, username));

    return true;
}

bool ChatServer::disconnect(Client *client) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    return session_manager_.close_session(client);
}

void ChatServer::post_chat_event(const ChatEvent& event) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    Wt::WApplication *app = Wt::WApplication::instance();

    auto clients = session_manager_.active_sessions();
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
    auto clients = session_manager_.active_sessions();
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
std::map<Wt::WString, Wt::WString> ChatServer::avatar_map() {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    return avatar_map_;
}

std::vector<chat::Dialogue> ChatServer::get_dialogues(uint user_id) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    return dialogue_service_.get_dialogues(user_id);
}

std::vector<chat::Message> ChatServer::get_messages(uint dialogue_id) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    return dialogue_service_.get_messages(dialogue_id);
}

int ChatServer::get_unread_messages_count(uint dialogue_id, uint user_id) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    return dialogue_service_.get_unread_messages_count(dialogue_id, user_id);
}

void ChatServer::send_msg(chat::Message& message, const chat::User& receiver) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    dialogue_service_.post_message(message);
    if (online_users_.count(receiver.username)) {
        notify_user(ChatEvent(message, Wt::WString(receiver.username)));
    }
}

bool ChatServer::create_dialogue(uint creater_id, const Wt::WString& receiver) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    if (dialogue_service_.create_dialogue(creater_id, 
                                          get_user_id(receiver.toUTF8()))) {
        notify_user(ChatEvent(ChatEvent::NEW_DIALOGUE, receiver));
        return true;
    }
    return false;
}

void ChatServer::mark_message_as_read(const chat::Message& message) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    dialogue_service_.mark_message_as_read(message.message_id);
    if (online_users_.count(message.user.username)) {
        notify_user(ChatEvent(message, message.user.username, ChatEvent::READ_MESSAGE));
    }
}

uint ChatServer::get_user_id(const Wt::WString& username) {
    return session_manager_.user_id(username.toUTF8());
}

void ChatServer::set_cookie(const std::string& username, const std::string& cookie) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    online_users_.insert(username);
    session_manager_.set_cookie(username, cookie);

    post_chat_event(ChatEvent(ChatEvent::SIGN_IN, username));
}

std::string ChatServer::check_cookie(const std::string& cookie) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    for (auto& session : session_manager_.active_sessions()) {
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
//    if (session_manager_.has_reserved(username)) {
        // if there was no 'close_same_session' call
        if (session_manager_.active_sessions().find(client) != session_manager_.active_sessions().end()) {
            std::cout << "WEAK SIGN OUT"<< std::endl;
            online_users_.erase(username_);
            avatar_map_.erase(username_);
            post_chat_event(ChatEvent(ChatEvent::Type::SIGN_OUT, username));
        }
//    }
}

void ChatServer::close_same_session(const Wt::WString& username_) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    std::string username = username_.toUTF8();
    for (auto& session : session_manager_.active_sessions()) {
        if (session.second.username_ == username) {
            session_manager_.close_session(session.first);
            return;
        }
    }
}

UserModelPtr ChatServer::get_user_model(const Wt::WString &username) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    return user_manager_.get_user(username.toUTF8());
}

std::string ChatServer::get_user_picture(const Wt::WString &username, int accs_lvl) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    UserModelPtr user = user_manager_.get_user(username.toUTF8());
    db_session_.start_transaction();
    for (const auto& pic : user->pictures_) {
        if (pic->access_lvl_ == accs_lvl) {
            db_session_.end_transaction();
//            if (avatar_map_.find(username) == avatar_map().end()) {
//                avatar_map_[username] = pic->path_;
//            }
            return pic->path_;
        }
    }
    db_session_.end_transaction();
    const std::string default_avatar = "./default-avatar.png";
//    avatar_map_[username] = std::string();
    return default_avatar;
}
