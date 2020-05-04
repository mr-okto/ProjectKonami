#include "DialogueService.hpp"

namespace chat {
DialogueService::DialogueService(Cache& cache) {}
    //: cache_(cache) {}

std::vector<Dialogue> DialogueService::get_dialogues(unsigned int user_id) {
    return std::vector<Dialogue>();
}

Dialogue DialogueService::get_dealogue(unsigned int dialog_id) {
    return Dialogue();
}

std::vector<Message> DialogueService::get_messages(unsigned int dialog_id) {
    return std::vector<Message>();
}

Message DialogueService::post_message(const Message& message) {
    return Message();
}
}