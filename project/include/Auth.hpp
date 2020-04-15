#ifndef MESSENGER_PROJECT_AUTH_HPP
#define MESSENGER_PROJECT_AUTH_HPP

#include <string>
#include <utility>
#include <memory>

#include "SessionManager.hpp"

class UserModelPtr {};

class UserManager {
public:
    virtual UserModelPtr get_user(uint32_t id) = 0;
    virtual UserModelPtr get_user(const std::string &username) = 0;
};

struct AuthData {
    std::string login;
    std::string password;

public:
    explicit AuthData(std::string  login_, std::string  password_) :
        login(std::move(login_)),
        password(std::move(password_)) {};
};

class IAuth {
public:
    virtual std::string sign_in(const AuthData& data) = 0;
    virtual bool sign_out(std::string sessionToken) = 0;

    virtual bool sign_up(const AuthData& newData) = 0;
};

class ITokenGenerator {
public:
    virtual ~ITokenGenerator() = default;
    virtual std::string generate_token() = 0;
};

class Auth : public IAuth {
public:
    enum VerificationStatus {
        Valid,
        Invalid,
        Pending
    };

private:
    uint32_t current_user_id_;

    std::string current_login_;
    std::string current_password_;

    VerificationStatus current_verification_status_;

    UserManager& user_manager_;
    ITokenGenerator* token_generator_;
    SessionManager* session_manager_;

public:
    Auth() = delete;
    Auth(const Auth&) = delete;
    ~Auth() = default;

    explicit Auth(UserManager& user_manager, ITokenGenerator *generator, SessionManager* session_manager) :
        user_manager_(user_manager),
        token_generator_(generator),
        session_manager_(session_manager) {};

    std::string sign_in(const AuthData& data) override; // запрос в бд, генерация токена, создание Session и вызов SessionManager.add_session
    bool sign_out(std::string sessionToken) override;

    bool sign_up(const AuthData& newData) override;

private:
    bool check_data_from_db();

};


#endif //MESSENGER_PROJECT_AUTH_HPP
