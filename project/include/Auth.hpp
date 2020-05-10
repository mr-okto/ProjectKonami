#ifndef MESSENGER_PROJECT_AUTH_HPP
#define MESSENGER_PROJECT_AUTH_HPP

#include <string>
#include <utility>
#include <memory>

#include "SessionManager.hpp"
#include "Models.hpp"
#include "UserManager.hpp"

//struct AuthData {
//    std::string username;
//    std::string password;
//    uint32_t id;
//
//public:
//    AuthData() = default;
//
//     AuthData(std::string  login_, std::string  password_) :
//        username(std::move(login_)),
//        password(std::move(password_)) {};
//};

class IAuth {
public:
    virtual bool sign_in(const std::string& username,
                         const std::string& password,
                         uint32_t* id) = 0;
    virtual bool sign_out(std::string username) = 0;

    virtual bool sign_up(const std::string& username, const std::string& password) = 0;
};

class ITokenGenerator {
public:
    virtual ~ITokenGenerator() = default;
    virtual std::string generate_token() = 0;
};

class Auth : public IAuth {
private:
    uint32_t current_user_id_;

    std::string current_login_;
    std::string current_password_;

    UserManager<Wt::Dbo::backend::Postgres>& user_manager_;
    ITokenGenerator* token_generator_;
    SessionManager* session_manager_;

public:
    Auth() = default;
    Auth(const Auth&) = delete ;
    ~Auth() = default;

    explicit Auth(UserManager<Wt::Dbo::backend::Postgres>& user_manager, SessionManager* session_manager) :
            user_manager_(user_manager),
            session_manager_(session_manager) {};

    bool sign_in(const std::string& username,
                 const std::string& password,
                 uint32_t* id) override; // запрос в бд, генерация токена, создание Session и вызов SessionManager.add_session
    bool sign_out(std::string username) override;

    bool sign_up(const std::string& username, const std::string& password) override;

private:
    bool check_data_from_db();

};


#endif //MESSENGER_PROJECT_AUTH_HPP
