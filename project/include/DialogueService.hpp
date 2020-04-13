#pragma once

#include <time.h>
#include <vector>

namespace chat {
struct Content {};

struct Dialogue {
    unsigned int dialog_id;
    unsigned int sender_id;
    unsigned int receiver_id;
};

struct Message {
    unsigned int message_id;
    unsigned int dialog_id;
    unsigned int user_id;
    Content content;
    time_t time;
};

class DialogService {
 public:
    std::vector<Dialogue> get_dialogues(unsigned int user_id);
    Dialogue get_dealogue(unsigned int dialog_id);
    std::vector<Message> get_messages(unsigned int dialog_id, int start, int stop);
    Message post_message(const Message& message);
 private:
    //FIXME
};
}