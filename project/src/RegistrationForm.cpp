#include <Wt/WLineEdit.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WLabel.h>

#include "RegistrationForm.hpp"

RegistrationForm::RegistrationForm()
    :Wt::WContainerWidget(),
    is_valid_(true),
    error_(ErrorType::None),
    validator_()
{
    setFloatSide(Wt::Side::CenterX);
    create_UI();
}

void RegistrationForm::create_UI() {
    auto vLayout = setLayout(std::make_unique<Wt::WVBoxLayout>());
    vLayout->setContentsMargins(10,10,10,10);

    auto line = vLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0,  Wt::AlignmentFlag::Middle);
    auto label_ = std::make_unique<Wt::WLabel>("User name: ");
    auto label = label_.get();
    line->addWidget(std::move(label_));
    username_edit_ = line->addWidget(std::make_unique<Wt::WLineEdit>());
    username_edit_->setFloatSide(Wt::Side::Right);
    label->setBuddy(username_edit_);

    line = vLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0,  Wt::AlignmentFlag::Middle);
    line->addWidget(std::make_unique<Wt::WText>("Choose password: "));
    password_edit_first_ = line->addWidget(std::make_unique<Wt::WLineEdit>());
    password_edit_first_->setFloatSide(Wt::Side::Right);
    password_edit_first_->setAttributeValue("type", "password");

    line = vLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0,  Wt::AlignmentFlag::Middle);
    line->addWidget(std::make_unique<Wt::WText>("Repeat password: "));
    password_edit_second_ = line->addWidget(std::make_unique<Wt::WLineEdit>());
    password_edit_second_->setFloatSide(Wt::Side::Right);
    password_edit_second_->setAttributeValue("type", "password");

    line = vLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0,  Wt::AlignmentFlag::Middle);
    status_msg_ = line->addWidget(std::make_unique<Wt::WText>());
}

bool RegistrationForm::validate() {
    auto username = username_edit_->text();
    auto password1 = password_edit_first_->text();
    auto password2 = password_edit_second_->text();

    if (password1 != password2) {
        error_ = ErrorType::PasswordMismatch;
        is_valid_ = false;
        return false;
    }

    auto res = validator_.evaluate_strength(username, password1);

    if (res.isValid()) {
        is_valid_ = true;
        return true;
    } else if (!res.isValid()) {
        is_valid_ = false;
        status_msg_->setText(res.message().toUTF8());
        status_string_ = res.message();
        error_ = ErrorType::ShortPassword;
        return false;
    }
}

void RegistrationForm::set_user_exists_error() {
    error_ = ErrorType::UsernameExists;
    is_valid_ = false;
}
