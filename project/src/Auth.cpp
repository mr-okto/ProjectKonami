#include "Auth.hpp"

std::string Auth::sign_in(const AuthData &data) {
    return std::string();
}

bool Auth::sign_out(std::string sessionToken) {
    return false;
}

bool Auth::sign_up(const AuthData &newData) {
    return false;
}
