#include <Wt/WLineEdit.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WLabel.h>

#include "RegistrationForm.hpp"

RegistrationForm::RegistrationForm()
    :Wt::WContainerWidget(),
    is_valid_(false),
    error_(ErrorType::None)
{
    setFloatSide(Wt::Side::CenterX);
    create_UI();
}

void RegistrationForm::create_UI() {
    auto vLayout = setLayout(std::make_unique<Wt::WVBoxLayout>());
    vLayout->setContentsMargins(10,10,10,10);

    auto line = vLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0,  Wt::AlignmentFlag::Middle);
    line->addWidget(std::make_unique<Wt::WLabel>("User name: "));
    username_edit_ = line->addWidget(std::make_unique<Wt::WLineEdit>());
    username_edit_->setFloatSide(Wt::Side::Right);

    line = vLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0,  Wt::AlignmentFlag::Middle);
    line->addWidget(std::make_unique<Wt::WText>("Choose password: "));
    password_edit_first_ = line->addWidget(std::make_unique<Wt::WLineEdit>());
    password_edit_first_->setFloatSide(Wt::Side::Right);
    password_edit_first_->setAttributeValue("type", "password");

    line = vLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0,  Wt::AlignmentFlag::Middle);
    line->addWidget(std::make_unique<Wt::WText>("Repeat password: "));
    password_edit_second_ = line->addWidget(std::make_unique<Wt::WLineEdit>());
    password_edit_second_->setFloatSide(Wt::Side::Right);
    password_edit_first_->setAttributeValue("type", "password");

}

bool RegistrationForm::validate() {
    auto s = username_edit_->text().toUTF8();
//    if (s == "123") {
//        is_valid_ = true;
//    }
    is_valid_ = true;
    return true;
}

void RegistrationForm::set_user_exists_error() {
    error_ = ErrorType::UsernameExists;
    is_valid_ = false;
}
