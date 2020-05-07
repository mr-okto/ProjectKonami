#include "DialogueService.hpp"
#include <iostream>

namespace chat {
DialogueService::DialogueService(Cache& cache) {}
    //: cache_(cache) {}

std::vector<Dialogue> DialogueService::get_dialogues(const std::string& username) {
    if (dialogues_.count(username)) {
        return dialogues_[username];
    } else {
        return std::vector<Dialogue>();
    }
}

std::vector<Message> DialogueService::get_messages(uint dialogue_id) {
    if (messages_.count(dialogue_id)) {
        return messages_[dialogue_id];
    } else {
        return std::vector<Message>();
    }
}

bool DialogueService::create_dialogue(const chat::User& first_user, const chat::User& second_user) {
    if (dialogues_.count(first_user.username)) {
        for (const auto& dialogue: dialogues_[first_user.username]) {
            if (dialogue.first_user.username == first_user.username &&
                    dialogue.second_user.username == second_user.username) {
                return false;
            } else if (dialogue.first_user.username == second_user.username && 
                    dialogue.second_user.username == first_user.username) {
                return false;
            }
        }
        Dialogue tmp = {unique_dialogue_id, first_user, second_user};
        dialogues_[first_user.username].push_back(tmp);
    } else {
        std::vector<Dialogue> vec = {{unique_dialogue_id, first_user, second_user}};
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
        Dialogue tmp = {unique_dialogue_id, first_user, second_user};
        dialogues_[second_user.username].push_back(tmp);
    } else {
        std::vector<Dialogue> vec = {{unique_dialogue_id, first_user, second_user}};
        dialogues_[second_user.username] = vec;
    }
    unique_dialogue_id++;
    return true;
}

void DialogueService::post_message(const Message& message) {
    messages_[message.dialogue_id].push_back(message);
}
}