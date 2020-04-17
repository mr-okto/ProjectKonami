#include "ChatApplication.hpp"

ChatApplication::ChatApplication(const Wt::WEnvironment &env, ChatServer &server)
    : Wt::WApplication(env),
      server_(server),
      env_(env)
{
    setTitle("KonamiChat");
    useStyleSheet("chatapp");

//    root()->add
}
