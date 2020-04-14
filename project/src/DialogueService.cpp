#include "DialogueService.hpp"

namespace chat {
std::vector<Dialogue> DialogueService::get_dialogues(unsigned int user_id) {
    return std::vector<Dialogue>();
}

Dialogue DialogueService::get_dealogue(unsigned int dialog_id) {
    return Dialogue();
}

std::vector<Message> DialogueService::get_messages(unsigned int dialog_id, int start, int stop) {
    return std::vector<Message>();
}

Message DialogueService::post_message(const Message& message) {
    return Message();
}
}