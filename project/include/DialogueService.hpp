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
    bool is_read;
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

    ContentModel::Type parse_type(Content::FileType type);
    Content::FileType parse_type(ContentModel::Type type);

    std::vector<Dialogue> get_dialogues(const std::string& username);
    std::vector<Message> get_messages(uint dialog_id, const std::string& username);
    bool create_dialogue(uint first_user_id, uint second_user_id);
    void post_message(const Message& message);

 private:
    DbSession<Wt::Dbo::backend::Postgres>& session_;
    DialogueManager<Wt::Dbo::backend::Postgres> dialogue_manager_;
    MessageManager<Wt::Dbo::backend::Postgres> message_manager_;
    UserManager<Wt::Dbo::backend::Postgres> user_manager_;
};
}