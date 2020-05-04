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

bool DialogueService::create_dialogue(const std::string& first_username, const std::string& second_username) {
    std::cout << std::endl << "in create dealogue: " << first_username << " " << second_username << std::endl;
    if (dialogues_.count(first_username)) {
        for (const auto& item : dialogues_[first_username]) {
            if (item.username == second_username) {
                return false;
            }
        }
        Dialogue tmp = {unique_dialogue_id, second_username};
        dialogues_[first_username].push_back(tmp);
    } else {
        std::vector<Dialogue> vec = {{unique_dialogue_id, second_username}};
        dialogues_[first_username] = vec;
    }

    if (dialogues_.count(second_username)) {
        for (const auto& item : dialogues_[second_username]) {
            if (item.username == first_username) {
                return false;
            }
        }
        Dialogue tmp = {unique_dialogue_id, first_username};
        dialogues_[second_username].push_back(tmp);
    } else {
        std::vector<Dialogue> vec = {{unique_dialogue_id, first_username}};
        dialogues_[second_username] = vec;
    }
    unique_dialogue_id++;
    return true;
}

Message DialogueService::post_message(uint dialogue_id,
                                      const std::string& username, 
                                      const std::string& message_content) {
    Message message = {dialogue_id, username, message_content, time(NULL)};
    messages_[dialogue_id].push_back(message);
    return message;
}
}