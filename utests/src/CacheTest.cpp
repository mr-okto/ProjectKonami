#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "cache.hpp"
#include "moodels.hpp"

class MockSessionManager {
public:
    MOCK_METHOD0(get_online_users, std::vector<unsigned int>());
};

class TestCache : public  ::testing::Test {
protected:
    void SetUp() {
        MockSessionManager object;
        EXPECT_CALL(object, get_online_users()).Times(testing::AtLeast(1));
        test_cache = Cache();
    }
    Cache test_cache;
};

TEST_F(TestCache, TestGetDialogue) {
    Dialogue dialogue_to_test = test_cache.get_dialog(1);

    EXPECT_EQ(dialogue_to_test.dialogue_id, 1);
    EXPECT_EQ(dialogue_to_test.members[0]->user_id, 2);
    EXPECT_EQ(dialogue_to_test.members[1]->user_id, 1);
}

TEST_F(TestCache, Test) {
    std::vector<Dialogue> dialogues_to_test = test_cache.get_dialogues(2);

    EXPECT_EQ(dialogues_to_test.size(), 3);
    EXPECT_EQ(dialogues_to_test[0].dialogue_id, 2);
    EXPECT_EQ(dialogues_to_test[1].dialogue_id, 4);
    EXPECT_EQ(dialogues_to_test[2].dialogue_id, 5);
}

TEST_F(TestCache, TestPostMessage) {
    Dialogue dialogues_to_test = test_cache.get_dialog(1);
    int start_size = dialogues_to_test.messages.size();
    Message new_msg;
    test_cache.post_message(new_msg);

    dialogues_to_test = test_cache.get_dialog(1);

    EXPECT_EQ(start_size + 1, dialogues_to_test.messages.size());
}

TEST_F(TestCache, TestGetMessage) {
    std::vector<Message> test_msg = test_cache.get_messages(1, 0, 3);

    EXPECT_EQ(test_msg.size(), 3);
}
