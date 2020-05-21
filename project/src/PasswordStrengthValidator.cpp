#include <Wt/WString.h>

extern "C" {
    #include "passwdqc/passwdqc.h"
}

#include "PasswordStrengthValidator.hpp"

namespace {
    const char *reasons[] =
            { "ok",
              "error",
              "same",
              "similar",
              "short",
              "long",
              "simpleshort",
              "simple",
              "personal",
              "word",
              "seq"
            };
}

const int PasswordStrengthValidator::Disabled = std::numeric_limits<int>::max();

PasswordStrengthValidator::PasswordStrengthValidator() {
    min_length_[0] = Disabled;
    min_length_[1] = 7;
    min_length_[2] = 6;
    min_length_[3] = 4;
    min_length_[4] = 2;

    pass_phrase_word_ = 2;
    min_match_length_ = 3;
}

StrengthValidatorResult
PasswordStrengthValidator::evaluate_strength(const Wt::WString &login, const Wt::WString &password) const {
    passwdqc_params_qc_t params;
    for (unsigned i = 0; i < 5; ++i)
        params.min[i] = min_length_[i];

    params.passphrase_words = pass_phrase_word_;
    params.match_length = min_match_length_;
    params.similar_deny = false;
    params.random_bits = 0;
    params.max = 256;

    std::string login_utf8 = login.toUTF8();
    passwdqc_user_t user;
    user.pw_name = login_utf8.c_str();
//    user.pw_email = email.c_str();

    int index = passwdqc_check(&params, password.toUTF8().c_str(), nullptr, &user);
    Wt::WString message = reasons[index];

    bool valid = index == 0;
    StrengthValidatorResult result(valid, message,valid ? 5 : 0);

    return result;
}
