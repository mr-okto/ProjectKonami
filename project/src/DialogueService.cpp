#include "DialogueService.hpp"
#include <iostream>
#include <algorithm>

namespace chat {

std::vector<Dialogue> DialogueService::get_dialogues(const std::string& username) {
    /*if (dialogues_.count(username)) {
        auto dialogues = dialogues_[username];
        sort(dialogues.begin(), dialogues.end());
        return dialogues;
    } else {
        return std::vector<Dialogue>();
    }*/
    auto user = user_manager_.get_user(username);
    auto dialogues = user->dialogues_;
    std::vector<Dialogue> return_vec;
    session_.start_transaction();
    for (const auto& item : dialogues) {
        std::vector<User> users;
        for (const auto& member : item->members_) {
            users.push_back({(uint)member.id(), member->username_});
        }
        Dialogue dialogue = {(uint)item.id(), users[0], users[1], (time_t)0};
        return_vec.push_back(dialogue);
    }
    session_.end_transaction();
    return return_vec;
}

std::vector<Message> DialogueService::get_messages(uint dialogue_id) {
    auto messages = message_manager_.get_latest_messages(dialogue_id, 0);
    std::vector<Message> return_vec;
    session_.start_transaction();
    for (const auto& item : messages) {
        Content content = {Content::WITHOUTFILE, item->text_, "NULL"};
        User user = {(uint)item->author_.id(), item->author_->username_};
        Message message = {(uint)dialogue_id, user, content, item->creation_dt_};
        return_vec.push_back(message);
    }
    session_.end_transaction();

    return return_vec;
}

bool DialogueService::create_dialogue(const chat::User& first_user, const chat::User& second_user) {
    /*if (dialogues_.count(first_user.username)) {
        for (const auto& dialogue: dialogues_[first_user.username]) {
            if (dialogue.first_user.username == first_user.username &&
                    dialogue.second_user.username == second_user.username) {
                return false;
            } else if (dialogue.first_user.username == second_user.username && 
                    dialogue.second_user.username == first_user.username) {
                return false;
            }
        }
        Dialogue tmp = {unique_dialogue_id,
                        first_user, 
                        second_user, 
                        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
        dialogues_[first_user.username].push_back(tmp);
    } else {
        std::vector<Dialogue> vec = {{unique_dialogue_id, 
                                      first_user, 
                                      second_user, 
                                      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}};
        dialogues_[first_user.username] = vec;
    }

    if (dialogues_.count(second_user.username)) {
        for (const auto& dialogue : dialogues_[second_user.username]) {
            if (dialogue.first_user.username == first_user.username &&
                    dialogue.second_user.username == second_user.username) {
                return false;
            } else if (dialogue.first_user.username == second_user.username &&
                     dialogue.second_user.username == first_user.username) {
                return false;
            }
        }
        Dialogue tmp = {unique_dialogue_id, 
                        first_user, 
                        second_user, 
                        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
        dialogues_[second_user.username].push_back(tmp);
    } else {
        std::vector<Dialogue> vec = {{unique_dialogue_id, 
                                      first_user, 
                                      second_user, 
                                      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}};
        dialogues_[second_user.username] = vec;
    }
    unique_dialogue_id++;
    return true;*/
    dialogue_manager_.get_dialogue(first_user.user_id, second_user.user_id);
    return true;
}

void DialogueService::post_message(const Message& message) {
    /*messages_[message.dialogue_id].push_back(message);
    std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    for (auto& pairs : dialogues_) {
        for (auto& dialogue : pairs.second) {
            if (dialogue.dialogue_id == message.dialogue_id) {
                dialogue.last_msg_time = current_time;
            }
        }
    }*/
    message_manager_.create_msg(message.dialogue_id, message.user.user_id, message.content.message);
}
}