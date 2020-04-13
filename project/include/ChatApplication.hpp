#pragma once
#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "ChatServer.hpp"

class ChatApplication : public Wt::WApplication
{
public:
    ChatApplication(const Wt::WEnvironment& env, ChatServer& server);

private:
    ChatServer& server_;
    Wt::WLineEdit *nameEdit_;
    Wt::WText *greeting_;
};
