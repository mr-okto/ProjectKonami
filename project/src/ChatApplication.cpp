#include "ChatApplication.hpp"
#include "AuthWidget.hpp"

ChatApplication::ChatApplication(const Wt::WEnvironment &env, ChatServer &server)
    : Wt::WApplication(env),
      server_(server),
      env_(env)
{
    setTitle("KonamiChat");
    useStyleSheet("chatapp.css");

    messageResourceBundle().use(appRoot() + "simplechat");

    AuthWidget *chatWidget =
            root()->addWidget(Wt::cpp14::make_unique<AuthWidget>(server_));
    chatWidget->setStyleClass("chat");
//    chatWidget->setMaximumSize(Wt::WLength(400), Wt::WLength(500));
}
