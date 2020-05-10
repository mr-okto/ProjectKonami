#include "Auth.hpp"
#include <Wt/WApplication.h>

bool Auth::sign_in(const std::string& username,
                   const std::string& password,
                   uint32_t* id)

{
    if (!id) {
        throw std::runtime_error ("nullptr passed in an argument");
    }

    if (session_manager_->has_reserved(username)) {
        return false;
    }
    UserModelPtr storaged_user = user_manager_.get_user(username);

    if (storaged_user &&
            storaged_user->username_ == username &&
            storaged_user->pwd_hash_ == password)
    {

        *id = storaged_user.id();
        session_manager_->reserve(storaged_user->username_, storaged_user.id());

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
