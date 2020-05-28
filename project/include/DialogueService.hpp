#pragma once

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <ctime>
#include <chrono>
#include <vector>
#include <string>
#include <map>

#include "DbSession.hpp"
#include "DialogueManager.hpp"
#include "MessageManager.hpp"
#include "UserManager.hpp"
#include "Models.hpp"

typedef unsigned int uint;

namespace chat {
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
    uint messages_count;

    Dialogue() :
        dialogue_id(0),
        last_msg_time(0),
        messages_count(0) {}

    Dialogue(uint dialogue_id, const User& first_user, const User& second_user, uint messages_count, time_t last_msg_time=0) :
        dialogue_id(dialogue_id),
        first_user(first_user),
        second_user(second_user),
        last_msg_time(last_msg_time),
        messages_count(messages_count) {}
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

template <typename T>
class DialogueService {
 public:
    DialogueService(DbSession<T>& session) :
        session_(session),
        dialogue_manager_(session),
        message_manager_(session),
        user_manager_(session) {}

    ContentModel::Type parse_type(Content::FileType type);
    Content::FileType parse_type(ContentModel::Type type);

    std::vector<Dialogue> get_dialogues(uint user_id);
    std::vector<Message> get_messages(uint dialogue_id);
    int get_unread_messages_count(uint dialogue_id, uint user_id);
    std::pair<chat::Dialogue, bool> create_dialogue(uint first_user_id, uint second_user_id);
    void post_message(Message& message);
    void mark_message_as_read(uint message_id);

 private:
    DbSession<T>& session_;
    DialogueManager<T> dialogue_manager_;
    MessageManager<T> message_manager_;
    UserManager<T> user_manager_;
};

template <typename T>
ContentModel::Type DialogueService<T>::parse_type(Content::FileType type) {
    if (type == Content::IMAGE) {
        return ContentModel::IMAGE;
    } else if (type == Content::VIDEO) {
        return ContentModel::VIDEO;
    } else if (type == Content::DOCUMENT) {
        return ContentModel::DOCUMENT;  
    } else {
        return ContentModel::OTHER;
    }
}

template <typename T>
Content::FileType DialogueService<T>::parse_type(ContentModel::Type type) {
    if (type == ContentModel::IMAGE) {
        return Content::IMAGE;
    } else if (type == ContentModel::VIDEO) {
        return Content::VIDEO;
    } else if (type == ContentModel::DOCUMENT) {
        return Content::DOCUMENT;  
    } else {
        return Content::OTHER;
    }
}

template <typename T>
std::vector<Dialogue> DialogueService<T>::get_dialogues(uint user_id) {
    UserModelPtr user = user_manager_.get_user(user_id);
    Dialogues dialogues = user->dialogues_;
    std::vector<Dialogue> dualogues_vec;
    session_.start_transaction();
    for (const auto& item : dialogues) {
        std::vector<User> users;
        for (const auto& member : item->members_) {
            users.push_back(User((uint)member.id(), member->username_));
        }
        dualogues_vec.push_back(Dialogue((uint)item.id(), users[0], users[1], 0));
    }
    session_.end_transaction();
    session_.start_transaction();
    for (auto& item : dualogues_vec) {
        MessageModelPtr last_msg = message_manager_.get_last_msg(item.dialogue_id);
        if (last_msg) {
            item.last_msg_time = last_msg->creation_dt_;
        }
        item.messages_count = dialogue_manager_.count_messages(item.dialogue_id, user_id); 
    }
    session_.end_transaction();
    sort(dualogues_vec.begin(), dualogues_vec.end(), [](const Dialogue& lhs, const Dialogue& rhs) {
        return lhs.last_msg_time > rhs.last_msg_time;
    });

    return dualogues_vec;
}

template <typename T>
std::vector<Message> DialogueService<T>::get_messages(uint dialogue_id) {
    Messages messages = message_manager_.get_latest_messages(dialogue_id, 0);
    std::vector<Message> return_vec;
    session_.start_transaction();
    for (const auto& item : messages) {
        auto message_model = *item->content_elements_.begin();
        Content content(
            parse_type(message_model->type_),
            item->text_,
            message_model->file_path_
        );

        User user(
            (uint)item->author_.id(),
            item->author_->username_
        );

        Message message(
            (uint)dialogue_id, 
            user, 
            content, 
            item->creation_dt_,
            item->is_read_,
            (uint)message_model.id()
        );
        return_vec.push_back(message);
    }
    session_.end_transaction();
    return return_vec;
}

template <typename T>
std::pair<chat::Dialogue, bool> DialogueService<T>::create_dialogue(uint first_user_id, uint second_user_id) {
    session_.start_transaction();
    auto dialogue_iscreate = dialogue_manager_.get_or_create_dialogue(first_user_id, second_user_id);
    session_.end_transaction();
    session_.start_transaction();
    std::vector<User> users;
    for (const auto& member : std::get<0>(dialogue_iscreate)->members_) {
        users.push_back(User((uint)member.id(), member->username_));
    }
    session_.end_transaction();
    return {chat::Dialogue(std::get<0>(dialogue_iscreate).id(), users[0], users[1], 0), std::get<1>(dialogue_iscreate)};
}

template <typename T>
int DialogueService<T>::get_unread_messages_count(uint dialogue_id, uint user_id) {
    return dialogue_manager_.count_unread_messages(dialogue_id, user_id);
}

template <typename T>
void DialogueService<T>::post_message(Message& message) {
    MessageModelPtr message_model = message_manager_.create_msg(
        message.dialogue_id, message.user.user_id, message.content.message
    );
    message_manager_.add_content(
        message_model, parse_type(message.content.type), message.content.file_path
    );
    message.message_id = message_model.id();
}

template <typename T>
void DialogueService<T>::mark_message_as_read(uint message_id) {
    session_.start_transaction();
    message_manager_.mark_read(message_id);
    session_.end_transaction();
}
}