#pragma once

#include <time.h>
#include <vector>
#include <string>

namespace chat {
class Cache {};

typedef std::string Content;

struct Dialogue {
    unsigned int dialog_id;
    unsigned int sender_id;
    unsigned int receiver_id;
};

struct Message {
    unsigned int dialog_id;
    unsigned int user_id;
    Content content;
    time_t time;
};

static bool operator==(const Message& lhs, const Message& rhs) {
    if (lhs.content == rhs.content && lhs.time == rhs.time) {
        return true;
    }
    return false;
}

class DialogueService {
 public:
    DialogueService(Cache& cache);
    DialogueService() {}
    std::vector<Dialogue> get_dialogues(unsigned int user_id);
    Dialogue get_dealogue(unsigned int dialog_id);
    std::vector<Message> get_messages(unsigned int dialog_id);
    Message post_message(const Message& message);
 private:
    //Cache& cache_;
    //FIXME
};
}