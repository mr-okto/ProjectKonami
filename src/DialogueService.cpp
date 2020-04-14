#include "DialogueService.hpp"

namespace chat {
std::vector<Dialogue> DialogService::get_dialogues(unsigned int user_id) {
    return std::vector<Dialogue>();
}

Dialogue DialogService::get_dealogue(unsigned int dialog_id) {
    return Dialogue();
}

std::vector<Message> DialogService::get_messages(unsigned int dialog_id, int start, int stop) {
    return std::vector<Message>();
}

Message DialogService::post_message(const Message& message) {
    return Message();
}
}