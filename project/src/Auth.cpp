#include "Auth.hpp"
#include <Wt/WApplication.h>

bool Auth::sign_in(const std::string& username,
                   const std::string& password,
                   AuthData& data)
{
    UserModelPtr storaged_user = user_manager_.get_user(username);
    if (session_manager_->has_reserved(username)) {
        return false;
    }

    if (storaged_user &&
            storaged_user->username_ == username &&
            storaged_user->pwd_hash_ == password)
    {
//        data.id = storaged_user->id_;
//        data.username = username;
//        data.password = password;

        session_manager_->reserve(storaged_user->username_, storaged_user->id_);

        return true;
    } else
        return false;
}

bool Auth::sign_out(std::string username) {
    return false;
}

bool Auth::sign_up(const std::string& username, const std::string& password) {
    //(TODO) добавить фейк логику

    UserModelPtr storaged_user = user_manager_.create_user(username, password);

    if (storaged_user) {
        return true;
    } else
        return false;

}
