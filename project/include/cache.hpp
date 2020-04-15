#pragma once

#include <iostream>
#include <vector>

#include "models.hpp"

#define INIT_SIZE 10
#define RESIZE_RATIO 2
#define LAST_DIALOGUE_IN_CACHE 5

class Cache {
public:
    Cache();
    Cache(std::vector<uint32_t>);

    ~Cache();
    std::vector<Dialogue> get_dialogues(uint32_t user_id);
    Dialogue get_dialog(uint32_t dialogue_id);
    std::vector<Message> get_messages(uint32_t dialogue_id, int start, int stop);
    int post_message(const Message& message);
    void update_cache();

private:
    void add_dialog_to_user(uint32_t user_id, Dialogue *dialogue);
    std::vector<User> active_users;
    std::vector<Dialogue> active_dialogue;
};

