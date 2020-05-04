#include "DialogueService.hpp"

namespace chat {
DialogueService::DialogueService(Cache& cache) {}
    //: cache_(cache) {}

std::vector<Dialogue> DialogueService::get_dialogues(const std::string& username) {
    return dialogues_[username];
}

std::vector<Message> DialogueService::get_messages(uint dialogue_id) {
    return messages_[dialogue_id];
}

Message DialogueService::post_message(uint dialogue_id,
                                      const std::string& username, 
                                      const std::string& message_content) {
    Message message = {dialogue_id, username, message_content, time(NULL)};
    messages_[dialogue_id].push_back(message);
    return message;
}
}