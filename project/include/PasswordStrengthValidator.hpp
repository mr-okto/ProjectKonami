#ifndef PROJECTKONAMI_PASSWORDSTRENGTHVALIDATOR_HPP
#define PROJECTKONAMI_PASSWORDSTRENGTHVALIDATOR_HPP

#include <Wt/WString.h>

class StrengthValidatorResult {
public:
    StrengthValidatorResult(bool valid,
                            const Wt::WString &message,
                            int strength)
                        : valid_(valid),
                          message_(message),
                          strength_(strength) {}

    bool isValid() const { return valid_; }

    Wt::WString message() { return message_; }

    int strength() const { return strength_; }

private:
    bool valid_;
    Wt::WString message_;
    int strength_;
};

class PasswordStrengthValidator {
public:
    static const int Disabled;

    PasswordStrengthValidator();

    StrengthValidatorResult
    evaluate_strength(const Wt::WString& login, const Wt::WString& password) const;

private:
    int min_length_[5];
    int pass_phrase_word_;
    int min_match_length_;
};


#endif //PROJECTKONAMI_PASSWORDSTRENGTHVALIDATOR_HPP
