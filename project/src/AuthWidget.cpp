#include <Wt/WContainerWidget.h>
#include <Wt/WEnvironment.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/WEvent.h>

#include "AuthWidget.hpp"

AuthWidget::AuthWidget(ChatServer &server)
    : WContainerWidget(),
      server_(server),
      signed_in_(false)
{
    //(TODO) ther're will be checking sessionId from cookie

    let_sign_in();
}

AuthWidget::~AuthWidget() {

}

void AuthWidget::let_sign_in() {
    clear();

    auto vLayout = setLayout(std::make_unique<Wt::WVBoxLayout>());

    auto hLayout_(std::make_unique<Wt::WHBoxLayout>());
    auto hLayout = hLayout_.get();

    vLayout->addLayout(std::move(hLayout_), 1, Wt::AlignmentFlag::Top | Wt::AlignmentFlag::Middle);
    hLayout->addWidget(std::make_unique<Wt::WLabel>("Login:"), 0, Wt::AlignmentFlag::Middle);

    username_edit_field_ = hLayout->addWidget(std::make_unique<Wt::WLineEdit>(username_), 0, Wt::AlignmentFlag::Middle);
    username_edit_field_->setFocus();

    auto hLayout1_(std::make_unique<Wt::WHBoxLayout>());
    auto hLayout1 = hLayout1_.get();

    vLayout->addLayout(std::move(hLayout1_), 1, Wt::AlignmentFlag::Top | Wt::AlignmentFlag::Middle);
    hLayout1->addWidget(std::make_unique<Wt::WLabel>("Password:"), 0, Wt::AlignmentFlag::Middle);

    password_edit_field_ = hLayout1->addWidget(std::make_unique<Wt::WLineEdit>(password_), 0, Wt::AlignmentFlag::Middle);
    password_edit_field_->setFocus();

    auto buttons_(std::make_unique<Wt::WHBoxLayout>());
    auto buttons = buttons_.get();
    vLayout->addLayout(std::move(buttons_));

    auto signInButton = buttons->addWidget(std::make_unique<Wt::WPushButton>("Sign in"), 1,
            Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Bottom);
    signInButton->clicked().connect(this, &AuthWidget::sign_in);
    signInButton->setFloatSide(Wt::Side::Bottom);
    username_edit_field_->enterPressed().connect(this, &AuthWidget::sign_in);
    password_edit_field_->enterPressed().connect(this, &AuthWidget::sign_in);

    auto signUpButton = buttons->addWidget(std::make_unique<Wt::WPushButton>("Sign up"));
    signUpButton->clicked().connect(this, &AuthWidget::show_registration);

    status_msg_ = vLayout->addWidget(Wt::cpp14::make_unique<Wt::WText>(), 0);
    status_msg_->setTextFormat(Wt::TextFormat::Plain);
}

void AuthWidget::sign_in() {
    if (!signed_in()) {
        Wt::WString username = username_edit_field_->text();
        Wt::WString password = password_edit_field_->text();

        if (!start_chat(username, password)) {
            status_msg_->setText("Incorrect login - '" + escapeText(username) +
                                 "' or password.");
        }
    }
}

bool AuthWidget::start_chat(const Wt::WString& username, const Wt::WString& password) {
    //(TODO) There will be created start point of ChatWidget
    // connect to server, create session, pass callbackFunc, manage session
    if (server_.sign_in(username, password)) {
        signed_in_ = true;

        session_signal_.emit(username);

        return true;
    } else {
        return false;
    }

}

void AuthWidget::show_registration() {
    Wt::WDialog dialog("Registration");
    dialog.setClosable(true);
    dialog.rejectWhenEscapePressed(true);
    dialog.contents()->setStyleClass("registration");

    registration_form_ = dialog.contents()->addWidget(std::make_unique<RegistrationForm>());
    registration_form_->setStyleClass("registration-form");
    Wt::WPushButton *signUpbButton = dialog.footer()->addWidget(std::make_unique<Wt::WPushButton>("Sign up"));
    Wt::WText *statusMsg = dialog.footer()->addWidget(std::make_unique<Wt::WText>());
    statusMsg->setTextFormat(Wt::TextFormat::Plain);

//    registration_form_->validate();
    auto s = registration_form_->get_username().toUTF8();
    if (s == "123") {
        std::cout << 123;
    }
    signUpbButton->clicked().connect(this, &AuthWidget::sign_up);
    signUpbButton->clicked().connect(std::bind(&AuthWidget::validate_reg_dialog, this, std::ref(dialog), statusMsg));
    //    signUpbButton->clicked().connect(&dialog, &Wt::WDialog::accept);

    dialog.exec();
}

void AuthWidget::sign_up() {
    if (registration_form_->validate()) {
        if (!server_.sign_up(registration_form_->get_username(), registration_form_->get_password_first())) {
            registration_form_->set_user_exists_error();
        }
    }

}

void AuthWidget::validate_reg_dialog(Wt::WDialog &dialog, Wt::WText* status_msg) {
    if (registration_form_->is_valid()) {
        dialog.accept();
    } else if (registration_form_->error() == RegistrationForm::ErrorType::PasswordsMismatch) {
        status_msg->setText("Passwords do not match.");
    } else if (registration_form_->error() == RegistrationForm::ErrorType::ShortPassword) {
        status_msg->setText("Passwords is too short.");
    } else if (registration_form_->error() == RegistrationForm::ErrorType::UsernameExists) {
        status_msg->setText("Username '" + escapeText(registration_form_->get_username()) + "' is already taken");
    }
}
