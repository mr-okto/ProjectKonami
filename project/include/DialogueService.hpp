#pragma once

#include <ctime>
#include <chrono>
#include <vector>
#include <string>
#include <map>

typedef unsigned int uint;

namespace chat {
class Cache {};

typedef std::string Content;

struct Dialogue {
    uint dialog_id;
    std::string username;
};

struct Message {
    uint dialog_id;
    std::string username;
    Content content;
    std::time_t time;
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
    DialogueService() : dialogues_({
                                    {"bob", {{2, "lel"}, {3, "kek"}}},
                                    {"lel",  {{1, "kek"}, {2, "bob"}}}, 
                                    {"kek", {{1, "lel"}, {3, "bob"}}}
                                   }),
                        messages_({  
                                   {1, {{1, "kek", "message for lel", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}, 
                                        {1, "lel", "message for kek", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}}},
                                   {2, {{2, "bob", "message for lel", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}}},
                                   {3, {{3, "kek", "message for bob", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}}}
                                  }),
                        unique_dialogue_id(4) {}
    std::vector<Dialogue> get_dialogues(const std::string& username);
    std::vector<Message> get_messages(uint dialog_id);
    bool create_dialogue(const std::string& first_username, const std::string& second_username);
    Message post_message(uint dialogue_id,
                         const std::string& username, 
                         const std::string& message_content);
 private:
    std::map<std::string, std::vector<Dialogue>> dialogues_;
    std::map<uint, std::vector<Message>> messages_;
    uint unique_dialogue_id;
    //Cache& cache_;
    //FIXME
};
}