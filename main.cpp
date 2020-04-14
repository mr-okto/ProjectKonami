#include <iostream>
#include <string>

#include "Auth.hpp"
#include "SessionManager.hpp"
#include "Session.hpp"

class TokenGenerator : public ITokenGenerator {
public:
    std::string generate_token() override { return std::string();};
};

int main() {

    SessionManager sessionManager1;
    TokenGenerator tokenGenerator;

    Auth auth(&tokenGenerator, &sessionManager1);

    return 0;
}
