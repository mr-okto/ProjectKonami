#include <iostream>
#include <vector>

#include "cache.hpp"
#include "models.hpp"

#define INIT_SIZE 10
#define RESIZE_RATIO 2
#define LAST_DIALOGUE_IN_CACHE 5


Cache::Cache() {
    active_users.resize(INIT_SIZE);
    active_dialogue.resize(INIT_SIZE * INIT_SIZE);
}

Cache::~Cache() {
    for (auto & active_user : active_users) {
        active_user.dialogues.clear();
    }
    active_users.clear();
    active_dialogue.clear();
}

std::vector<Dialogue> Cache::get_dialogues(uint32_t user_id) {
    std::vector<Dialogue> result;
    for (auto & active_user : active_users) {
        if (active_user.user_id == user_id) {
            result.resize(active_user.dialogues.size());
            for (int j = 0; j < active_user.dialogues.size(); ++j) {
                result[j] = *active_user.dialogues[j];
            }
            return result;
        }
    }
    return result;
}


Dialogue Cache::get_dialog(uint32_t dialogue_id) {
    for (auto dialogue: active_dialogue) {
        if (dialogue.dialogue_id == dialogue_id)
            return dialogue;
    }
    Dialogue new_active_dialogue; // Получить диалог из базы данных и добавить его в кэш get_dialog_from_db(dialogue_id);
    if (active_dialogue.size() == active_dialogue.capacity()) {
        active_dialogue.resize(active_dialogue.size() * RESIZE_RATIO);
    }
    active_dialogue.push_back(new_active_dialogue);

    for (auto & member : new_active_dialogue.members) {
        add_dialog_to_user(member->user_id, &active_dialogue.back());
    }

    return new_active_dialogue;
}

int Cache::post_message(const Message& message) {
    // Загрузить сообщение в БД upload_msg_to_db(message);
    uint32_t dialogue_id = message.dialogue->dialogue_id; // Получить id диалога в окторый нужно дабавить
    for (auto & dialogue : active_dialogue) {
        if (dialogue.dialogue_id == dialogue_id) {
            if (dialogue.messages.size() == dialogue.messages.capacity()) {
                dialogue.messages.resize(dialogue.messages.size() * RESIZE_RATIO);
            }
            std::vector<Message>::iterator it;
            it = dialogue.messages.begin();
            dialogue.messages.insert(it, message);
            return 0;
        }
    }
    return 1;
}

std::vector<Message> Cache::get_messages(uint32_t dialogue_id, int start, int stop) {
    for (const auto& dialogue : active_dialogue) {
        if (dialogue.dialogue_id == dialogue_id) {
            std::vector<Message> messages(stop - start);
            //copy(start, stop, messages);
            return messages;
        }
    }
    return std::vector<Message>();
}

void Cache::update_cache() {
    std::vector<User> new_active_users; // get_online_users;
    for (int i = 0; i < active_dialogue.size(); i++) {
        bool dialogue_have_active_user = false;
        for (const auto& user : new_active_users) {
            for (auto member : active_dialogue[i].members) {
                if (user.user_id == member->user_id) {
                    dialogue_have_active_user = true;
                    break;
                }
            }
            if (dialogue_have_active_user) {
                break;
            }
        }
        if (!dialogue_have_active_user) {
            new_active_users.erase(new_active_users.begin() + i);
            i--;
        }
    }
}

void Cache::add_dialog_to_user(uint32_t user_id, Dialogue *dialogue) {
    for (auto & active_user : active_users) {
        if (user_id == active_user.user_id) {
            if (active_user.dialogues.capacity() == active_user.dialogues.size()) {
                active_user.dialogues.resize(active_user.dialogues.size() * RESIZE_RATIO);
            }
            active_user.dialogues.push_back(dialogue);
        }
    }
}