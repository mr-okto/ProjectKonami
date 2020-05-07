#ifndef PROJECTKONAMI_AUTHWIDGET_HPP
#define PROJECTKONAMI_AUTHWIDGET_HPP

#include <Wt/WContainerWidget.h>
#include <Wt/WMessageBox.h>
#include <Wt/WSignal.h>
#include <Wt/WCheckBox.h>

#include "ChatServer.hpp"
#include "RegistrationForm.hpp"

struct Session;
class ChatEvent;

class AuthWidget : public Wt::WContainerWidget {
public:
    AuthWidget(ChatServer& server);
    ~AuthWidget();


    // show sign_in screen
    void create_UI();

    bool start_chat(const Wt::WString& username, const Wt::WString& password);

    Wt::Signal<Wt::WString, std::optional<std::string>>& session_signal() { return session_signal_; }

protected:
    bool signed_in() const { return signed_in_; }

private:
    ChatServer& server_;
    bool signed_in_;
    Wt::Signal<Wt::WString, std::optional<std::string>> session_signal_;

    Wt::WString username_;
    Wt::WLineEdit *username_edit_field_;

    Wt::WString password_;
    Wt::WLineEdit *password_edit_field_;

    RegistrationForm *registration_form_;
    Wt::WCheckBox *remember_me_box_;

    Wt::WText *status_msg_;

    std::unique_ptr<Wt::WVBoxLayout> create_input_forms_layout();

    void sign_in() ;
    void show_registration();
    void sign_up() ;

    void validate_reg_dialog(Wt::WDialog& dialog, Wt::WText* status_msg);

};


#endif //PROJECTKONAMI_AUTHWIDGET_HPP
