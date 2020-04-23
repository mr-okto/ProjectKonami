#ifndef PROJECTKONAMI_AUTHWIDGET_HPP
#define PROJECTKONAMI_AUTHWIDGET_HPP

#include <Wt/WContainerWidget.h>
#include <Wt/WMessageBox.h>

#include "ChatServer.hpp"
#include "RegistrationForm.hpp"


class ChatEvent;

class AuthWidget : public Wt::WContainerWidget, public Client {
public:
    AuthWidget(ChatServer& server);
    ~AuthWidget();

    void connect() override ;
    void disconnect() override ;

    // show sign_in screen
    void let_sign_in();

    void sign_out() override {};

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

    RegistrationForm *registration_form_;

    uint32_t  user_id_;

    Wt::WText *status_msg_;

    void sign_in() override;
    void show_registration();
    void sign_up() override;

    void validate_reg_dialog(Wt::WDialog& dialog, Wt::WText* status_msg);

};


#endif //PROJECTKONAMI_AUTHWIDGET_HPP
