#include "Auth.hpp"

bool Auth::sign_in(const std::string& username,
                   const std::string& password,
                   AuthData& data)
{
    UserModelPtr storaged_user = user_manager_.get_user(username);

    if (storaged_user &&
            storaged_user->username_ == username &&
            storaged_user->pwd_hash_ == password)
    {
        data.id = storaged_user->id_;
        data.username = username;
        data.password = password;
        return true;
    } else
        return false;
}

bool Auth::sign_out(std::string sessionToken) {
    return false;
}

bool Auth::sign_up(const AuthData &newData) {
    return false;
}
