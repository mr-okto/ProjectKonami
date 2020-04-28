#include <Wt/WSignal.h>

#include "ChatApplication.hpp"
#include "AuthWidget.hpp"
#include "ChatWidget.hpp"

ChatApplication::ChatApplication(const Wt::WEnvironment &env, ChatServer &server)
    : Wt::WApplication(env),
      server_(server),
      env_(env)
{
    std::cout << "SESSION ID" << Wt::WApplication::instance()->sessionId() << "  " << this << std::endl;
    setTitle("KonamiChat");
    useStyleSheet("chatapp.css");

    messageResourceBundle().use(appRoot() + "simplechat");

    AuthWidget *authWidget =
            root()->addWidget(Wt::cpp14::make_unique<AuthWidget>(server_));
    authWidget->setStyleClass("chat");
//    chatWidget->setMaximumSize(Wt::WLength(400), Wt::WLength(500));
    authWidget->session_signal().connect(this, &ChatApplication::start_chat);

}

void ChatApplication::start_chat(const Wt::WString& username) {
    root()->clear();

    ChatWidget* chatWidget =
            root()->addWidget(std::make_unique<ChatWidget>(username, server_));
    chatWidget->setStyleClass("chat");
}
