#include <Wt/WSignal.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WEnvironment.h>

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
//    std::cout << "SESSION ID" << Wt::WApplication::instance()->sessionId() << "  " << this << std::endl;
    setTitle("KonamiChat");

    useStyleSheet("chatapp.css");
    messageResourceBundle().use(appRoot() + "simplechat");

    for (auto& i : env_.cookies()) {
        std::cout << i.first << " : " << i.second << std::endl;
    }

    start_auth();
}

void ChatApplication::start_chat(const Wt::WString& username, uint32_t id,
                                const std::optional<std::string>& cookie)
{
    root()->clear();

    logged_in_ = true;
    ChatWidget *chatWidget = nullptr;
    if (!cookie.has_value()) {
        std::cout << "without cookie" << std::endl;
        chatWidget =
                root()->addWidget(std::make_unique<ChatWidget>(username, id, server_));
    } else {
        std::cout << "with cookie" << std::endl;
        chatWidget =
                root()->addWidget(std::make_unique<ChatWidget>(username, id, cookie, server_));
    }
    chatWidget->setStyleClass("chat-main");
    chatWidget->logout_signal().connect(this, &ChatApplication::start_auth);
}

void ChatApplication::start_auth() {
    root()->clear();

    auto cookie = env_.getCookie("username");
    if (cookie && !cookie->empty()) {
        std::string username = server_.check_cookie(*cookie);
        if (!username.empty()) {
            std::cout << username << " : " << *cookie << std::endl;
            start_chat(username, std::stoi(std::string(*cookie)) , *cookie);
            return;
        }
    }

    logged_in_ = false;
    AuthWidget *authWidget =
            root()->addWidget(Wt::cpp14::make_unique<AuthWidget>(server_));
    authWidget->setStyleClass("auth");
    authWidget->session_signal().connect(
            [this](const std::pair<Wt::WString, uint32_t>& data,
                           const std::optional<std::string>& cookie)
    {
        start_chat(data.first, data.second, cookie);
    });
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