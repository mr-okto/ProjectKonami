#pragma once

#include <iostream>
#include <vector>

class Dialogue;
class User;
class Message;

class Message{
public:
    uint32_t message_id;
    std::string text;
    User *author;
    Dialogue *dialogue;
};

class User{
public:
    uint32_t user_id;
    std::string pwd_hash;
    std::string user_name;
    std::vector<Dialogue *> dialogues;
};


class Dialogue{
public:
    uint32_t  dialogue_id = 0;
    std::vector<User *> members;
    std::vector<Message> messages;
};
