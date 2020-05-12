#pragma once

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <DbSession.hpp>
#include <DialogueManager.hpp>
#include <MessageManager.hpp>
#include <UserManager.hpp>
#include <Models.hpp>

#include <ctime>
#include <chrono>
#include <vector>
#include <string>
#include <map>

typedef unsigned int uint;

namespace chat {
class Cache {};

struct Content {
    enum FileType {
        IMAGE,
        VIDEO,
        DOCUMENT,
        OTHER,
    };
    FileType type;
    std::string message;
    std::string file_path;

    explicit Content(const std::string& message = "") :
        type(OTHER),
        message(message),
        file_path("NULL") {}

    Content(FileType type, const std::string& message, const std::string& file_path) :
        type(type),
        message(message),
        file_path(file_path) {}
};

struct User {
    uint user_id;
    std::string username;

    User() : 
        user_id(0),
        username("") {}

    User(uint user_id, const std::string& username) :
        user_id(user_id),
        username(username) {}
};

struct Dialogue {
    uint dialogue_id;
    User first_user;
    User second_user;
    std::time_t last_msg_time;

    Dialogue() :
        dialogue_id(0),
        last_msg_time(0) {}

    Dialogue(uint dialogue_id, const User& first_user, const User& second_user, time_t last_msg_time=0) :
        dialogue_id(dialogue_id),
        first_user(first_user),
        second_user(second_user),
        last_msg_time(last_msg_time) {}

};

struct Message {
    uint dialogue_id;
    User user;
    Content content;
    std::time_t time;
    bool is_read;
    uint message_id;

    Message() : 
        dialogue_id(0),
        time(0),
        is_read(false),
        message_id(0) {}

    Message(uint dialogue_id, const User& user, const Content& content) :
        dialogue_id(dialogue_id),
        user(user),
        content(content),
        time(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())),
        is_read(false),
        message_id(0) {}

    Message(uint dialogue_id, 
            const User& user, 
            const Content& content, 
            time_t time, 
            bool is_read,
            uint message_id) :
        dialogue_id(dialogue_id),
        user(user),
        content(content),
        time(time),
        is_read(is_read),
        message_id(message_id) {}
};

static bool operator==(const Message& lhs, const Message& rhs) {
    if (lhs.time == rhs.time) {
        return true;
    }
    return false;
}

static bool operator<(const Dialogue& lhs, const Dialogue& rhs) {
    return lhs.last_msg_time > rhs.last_msg_time;
}

static bool operator<(const Message& lhs, const Message& rhs) {
    return lhs.time < rhs.time;
}

class DialogueService {
 public:
    DialogueService(DbSession<Wt::Dbo::backend::Postgres>& session) :
        session_(session),
        dialogue_manager_(session),
        message_manager_(session),
        user_manager_(session) {}

    ContentModel::Type parse_type(Content::FileType type);
    Content::FileType parse_type(ContentModel::Type type);

    std::vector<Dialogue> get_dialogues(const std::string& username);
    std::vector<Message> get_messages(uint dialog_id, const std::string& username);
    bool create_dialogue(uint first_user_id, uint second_user_id);
    Message post_message(const Message& message);
    void mark_message_as_read(uint message_id);

 private:
    DbSession<Wt::Dbo::backend::Postgres>& session_;
    DialogueManager<Wt::Dbo::backend::Postgres> dialogue_manager_;
    MessageManager<Wt::Dbo::backend::Postgres> message_manager_;
    UserManager<Wt::Dbo::backend::Postgres> user_manager_;
};
}