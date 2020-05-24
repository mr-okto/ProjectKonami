#include "DialogueService.hpp"
#include <iostream>
#include <algorithm>
#include <utility>

namespace chat {

ContentModel::Type DialogueService::parse_type(Content::FileType type) {
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

Content::FileType DialogueService::parse_type(ContentModel::Type type) {
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

std::vector<Dialogue> DialogueService::get_dialogues(const std::string& username) {
    auto user = user_manager_.get_user(username);
    auto dialogues = user->dialogues_;
    std::vector<Dialogue> return_vec;
    session_.start_transaction();
    for (const auto& item : dialogues) {
        std::vector<User> users;
        for (const auto& member : item->members_) {
            users.push_back(User((uint)member.id(), member->username_));
        }
        return_vec.push_back(Dialogue((uint)item.id(), users[0], users[1]));
    }
    session_.end_transaction();
    session_.start_transaction();
    for (auto& item : return_vec) {
        auto last_msg = message_manager_.get_last_msg(item.dialogue_id);
        if (last_msg) {
            item.last_msg_time = last_msg->creation_dt_;
        }
            
    }
    session_.end_transaction();
    sort(return_vec.begin(), return_vec.end());
    return return_vec;
}

std::vector<Message> DialogueService::get_messages(uint dialogue_id, const std::string& username) {
    auto messages = message_manager_.get_latest_messages(dialogue_id, 0);
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

bool DialogueService::create_dialogue(uint first_user_id, uint second_user_id) {
    return std::get<1>(
        dialogue_manager_.get_or_create_dialogue(first_user_id, second_user_id)
    );
}

int DialogueService::get_unread_messages_count(uint dialogue_id, uint user_id) {
    return dialogue_manager_.count_unread_messages(dialogue_id, user_id);
}

void DialogueService::post_message(Message& message) {
    auto message_model = message_manager_.create_msg(
        message.dialogue_id, message.user.user_id, message.content.message
    );
    message_manager_.add_content(
        message_model, parse_type(message.content.type), message.content.file_path
    );
    message.message_id = message_model.id();
}

void DialogueService::mark_message_as_read(uint message_id) {
    session_.start_transaction();
    message_manager_.mark_read(message_id);
    session_.end_transaction();
}
}