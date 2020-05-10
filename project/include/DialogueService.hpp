#pragma once

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <DbSession.hpp>
#include <DialogueManager.hpp>
#include <MessageManager.hpp>
#include <UserManager.hpp>

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
        LOCATION,
        WITHOUTFILE,
    };
    FileType type;
    std::string message;
    std::string file_path;
};

struct User {
    uint user_id;
    std::string username;
};

struct Dialogue {
    uint dialogue_id;
    User first_user;
    User second_user;
    std::time_t last_msg_time;
};

struct Message {
    uint dialogue_id;
    User user;
    Content content;
    std::time_t time;
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

class DialogueService {
 public:
    DialogueService(DbSession<Wt::Dbo::backend::Postgres>& session) :
        session_(session),
        dialogue_manager_(session),
        message_manager_(session),
        user_manager_(session) {}

    std::vector<Dialogue> get_dialogues(const std::string& username);
    std::vector<Message> get_messages(uint dialog_id);
    bool create_dialogue(const chat::User& first_user, const chat::User& second_user);
    void post_message(const Message& message);

 private:
    //std::map<std::string, std::vector<Dialogue>> dialogues_;
    //std::map<uint, std::vector<Message>> messages_;

    DbSession<Wt::Dbo::backend::Postgres>& session_;
    DialogueManager<Wt::Dbo::backend::Postgres> dialogue_manager_;
    MessageManager<Wt::Dbo::backend::Postgres> message_manager_;
    UserManager<Wt::Dbo::backend::Postgres> user_manager_;
    //FIXME
};
}