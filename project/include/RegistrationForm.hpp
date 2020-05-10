#ifndef PROJECTKONAMI_REGISTRATIONFORM_HPP
#define PROJECTKONAMI_REGISTRATIONFORM_HPP

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>

#include "PasswordStrengthValidator.hpp"

struct RegistrationForm : public Wt::WContainerWidget {
public:
    RegistrationForm();

    bool is_valid() const { return is_valid_; }

    const Wt::WString& get_username() const { return username_edit_->text(); }
    const Wt::WString& get_password_first() const { return password_edit_first_->text(); }
    const Wt::WString& get_password_second() const { return password_edit_second_->text(); }

    //(TODO) add logic
    bool validate();



    enum ErrorType {
        None,
        ShortPassword,
        UsernameExists,
        PasswordMismatch
    };
    ErrorType error() const { return error_; }
    Wt::WString status() const { return status_string_;}

    void set_user_exists_error();

private:
    void create_UI();

    Wt::WLineEdit *username_edit_;
    Wt::WLineEdit *password_edit_first_;
    Wt::WLineEdit *password_edit_second_;

    bool is_valid_;
    ErrorType error_;

    PasswordStrengthValidator validator_;
    Wt::WText  *status_msg_;
    Wt::WString status_string_;
};


#endif //PROJECTKONAMI_REGISTRATIONFORM_HPP
