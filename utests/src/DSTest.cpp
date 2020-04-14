#include "DialogueService.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>


class MockCache {
 public:
    MockCache() {}
    MockCache(const MockCache&) = default;
    MockCache& operator=(const MockCache&) = default;

    MOCK_METHOD1(new_message, chat::Message(const chat::Message& message));
    MOCK_METHOD1(get_messages, std::vector<chat::Message>(unsigned int user_id));
    MOCK_METHOD1(get_dialogues, std::vector<chat::Dialogue>(unsigned int user_id));

 private:
};


TEST(DSPostMessageTEST, ok) {
    chat::Message msg;
    MockCache object;
    EXPECT_CALL(object, new_message(msg)).Times(testing::AtLeast(1));

    chat::DialogueService dialogue;
    dialogue.post_message(msg);
}

TEST(DSGetMessageTEST, ok) {
    int user_id = 1;
    MockCache object;
    EXPECT_CALL(object, get_messages(user_id)).Times(testing::AtLeast(1));

    chat::DialogueService dialogue;
    dialogue.get_messages(user_id, 1, 1);
}

TEST(DSGetDialogueTEST, ok) {
    int user_id = 3;
    MockCache object;
    EXPECT_CALL(object, get_dialogues(user_id)).Times(testing::AtLeast(1));

    chat::DialogueService dialogue;
    dialogue.get_dialogues(user_id);
}

