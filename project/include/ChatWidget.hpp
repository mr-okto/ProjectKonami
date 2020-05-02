#ifndef PROJECTKONAMI_CHATWIDGET_HPP
#define PROJECTKONAMI_CHATWIDGET_HPP

#include <Wt/WContainerWidget.h>
#include <Wt/WMessageBox.h>

#include "ChatServer.hpp"

class ChatWidget : public Wt::WContainerWidget, public Client {
public:
    ChatWidget(const Wt::WString& username, ChatServer& server);
    ~ChatWidget();

    void connect() override;
    void disconnect() override;

    void sign_out() override;

    std::string username() override { return username_.toUTF8(); }

private:
    ChatServer& server_;
    Wt::WString username_;

//    Session session_;
//    uint32_t  user_id_;

    Wt::JSlot                  clearInput_;

    Wt::WContainerWidget         *messages_;
    Wt::WTextArea                *messageEdit_;
    Wt::Core::observing_ptr<Wt::WPushButton> sendButton_;

    Wt::Core::observing_ptr<Wt::WContainerWidget> userList_;


    void create_UI();
    void create_layout(std::unique_ptr<Wt::WWidget> messages, std::unique_ptr<Wt::WWidget> userList,
                              std::unique_ptr<Wt::WWidget> messageEdit,
                              std::unique_ptr<Wt::WWidget> sendButton, std::unique_ptr<Wt::WWidget> logoutButton);

    void update_users_list();

    void process_chat_event(const ChatEvent& event);

};


#endif //PROJECTKONAMI_CHATWIDGET_HPP
