#include <Wt/WSignal.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WBootstrapTheme.h>

#include "ChatApplication.hpp"
#include "AuthWidget.hpp"
#include "ChatWidget.hpp"

//struct WrappedTimer : public Wt::WContainerWidget {
//    WrappedTimer(unsigned n = 2) : scheduler(n) {}
//    WrappedTimer(const WrappedTimer&) = delete;
//    ~WrappedTimer() = default;
//
//    Scheduler scheduler;
//};

ChatApplication::ChatApplication(const Wt::WEnvironment &env, ChatServer &server)
    : Wt::WApplication(env),
      server_(server),
      env_(env),
      logged_in_(false)
{
    std::cout << "SESSION ID" << Wt::WApplication::instance()->sessionId() << "  " << this << std::endl;
    setTitle("KonamiChat");

    useStyleSheet("chatapp.css");
    messageResourceBundle().use(appRoot() + "simplechat");

    start_auth();
}

void ChatApplication::start_chat(const Wt::WString& username) {
    root()->clear();

    logged_in_ = true;
    ChatWidget* chatWidget =
            root()->addWidget(std::make_unique<ChatWidget>(username, server_));
    chatWidget->setStyleClass("chat-main");
    chatWidget->logout_signal().connect(this, &ChatApplication::start_auth);
}

void ChatApplication::start_auth() {
    root()->clear();

    logged_in_ = false;
    AuthWidget *authWidget =
            root()->addWidget(Wt::cpp14::make_unique<AuthWidget>(server_));
    authWidget->setStyleClass("auth");
    authWidget->session_signal().connect(this, &ChatApplication::start_chat);
}

//void ChatApplication::idleTimeout() {
//    if (!logged_in_ || idle_timeout_dialog_) {
//        return;
//    }
//
//    idle_timeout_dialog_ = addChild(std::make_unique<Wt::WDialog>("Idle timeout!"));
//    idle_timeout_dialog_->contents()->addWidget(std::make_unique<Wt::WText>("This session will automatically quit in 1 minute, "
//                                                                            "press 'abort' to continue using the application"));
//
//    auto btn = idle_timeout_dialog_->footer()->addWidget(std::make_unique<Wt::WPushButton>("abort"));
//    btn->clicked().connect([this]() {
//        removeChild(idle_timeout_dialog_.get());
//    });
//
//    auto timer = idle_timeout_dialog_->contents()->addWidget(std::make_unique<WrappedTimer>());
//    timer->scheduler.in(std::chrono::seconds (60), [this]() {
//        quit();
//    });
//
//    idle_timeout_dialog_->show();
//}