#ifndef PROJECTKONAMI_CHATWIDGET_HPP
#define PROJECTKONAMI_CHATWIDGET_HPP

#include <Wt/WContainerWidget.h>

#include "ChatServer.hpp"

class ChatEvent;

class ChatWidget : public Wt::WContainerWidget, public Client {
public:
    ChatWidget(ChatServer& server);
    ~ChatWidget();

    void connect();
    void disconnect();

    // show sign_in screen
    void let_sign_in();

    bool start_chat(const Wt::WString& username, const Wt::WString& password);

protected:
    bool signed_in() const { return signed_in_; }

private:
    ChatServer& server_;
    bool signed_in_;

    Wt::WString username_;
    Wt::WLineEdit *username_edit_field_;

    Wt::WString password_;
    Wt::WLineEdit *password_edit_field_;

    uint32_t  user_id_;

    Wt::WText *status_msg_;

    void sign_in();

};


#endif //PROJECTKONAMI_CHATWIDGET_HPP
