#include <Wt/WServer.h>
#include <Wt/WString.h>
#include "ChatServer.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

struct AuthData {
    std::string login;
    std::string password;
    AuthData(std::string login="123", std::string password="123")
        : login(login), password(password) {}
};

bool operator==(const AuthData& lhs, const AuthData& rhs) {
    if (lhs.login == rhs.login && lhs.password == rhs.password) {
        return true;
    }
    return false;
}

class MockAuthSFA : public chat::AuthSFA {
 public:
    MOCK_METHOD1(sing_in, std::string(const AuthData& data));
    MOCK_METHOD1(sing_up, std::string(const AuthData& data));
    MOCK_METHOD1(sing_out, bool(std::string token));
};

TEST(ChatServerTest, ok) {
    Wt::WServer server(WTHTTP_CONFIGURATION);
    MockAuthSFA object;
    chat::ChatServer serv(server, object);
    EXPECT_CALL(object, sing_in(AuthData())).Times(testing::AtLeast(1));
    EXPECT_CALL(object, sing_up(AuthData())).Times(testing::AtLeast(1));
    EXPECT_CALL(object, sing_out("123")).Times(testing::AtLeast(1));

    serv.sing_in(Wt::WString("123"), Wt::WString("123"));
    serv.sing_up(Wt::WString("123"), Wt::WString("123"));
    serv.sing_out(Wt::WString("123"));
}
