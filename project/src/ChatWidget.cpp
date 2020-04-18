#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WEnvironment.h>
#include <Wt/WInPlaceEdit.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WCheckBox.h>

#include "ChatWidget.hpp"

ChatWidget::ChatWidget(ChatServer &server)
    : WContainerWidget(),
      server_(server),
      signed_in_(false)
{
    let_sign_in();
}

ChatWidget::~ChatWidget() {

}

void ChatWidget::let_sign_in() {
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

    auto button = vLayout->addWidget(std::make_unique<Wt::WPushButton>("Sign in"), 1,
            Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Bottom);
    button->clicked().connect(this, &ChatWidget::sign_in);
    button->setFloatSide(Wt::Side::Bottom);
    username_edit_field_->enterPressed().connect(this, &ChatWidget::sign_in);
    password_edit_field_->enterPressed().connect(this, &ChatWidget::sign_in);


    status_msg_ = vLayout->addWidget(Wt::cpp14::make_unique<Wt::WText>(), 0);
    status_msg_->setTextFormat(Wt::TextFormat::Plain);
}

void ChatWidget::sign_in() {
    if (!signed_in()) {
        Wt::WString username = username_edit_field_->text();
        Wt::WString password = password_edit_field_->text();

        if (!start_chat(username, password)) {
            status_msg_->setText("Sorry, name '" + escapeText(username) +
                                 "' is already taken.");
        }
    }
}

bool ChatWidget::start_chat(const Wt::WString& username, const Wt::WString& password) {
    if (server_.sign_in(username, password)) {
        signed_in_ = true;

        clear();
        auto layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
        layout->addWidget(std::make_unique<Wt::WText>("LOGINED"));

        return true;
    } else {
        return false;
    }

}
