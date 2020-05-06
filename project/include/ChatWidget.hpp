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
    Wt::Core::observing_ptr<Wt::WContainerWidget> dialoguesList_;

    std::map<Wt::WString, uint> dialogue_id_;
    Wt::WString current_dialogue_;


    void create_UI();
    void create_layout(std::unique_ptr<Wt::WWidget> messages, 
                    std::unique_ptr<Wt::WWidget> userList,
                    std::unique_ptr<Wt::WWidget> messageEdit,
                    std::unique_ptr<Wt::WWidget> sendButton, 
                    std::unique_ptr<Wt::WWidget> logoutButton,
                    std::unique_ptr<Wt::WWidget> chatUserList);

    void process_chat_event(const ChatEvent& event);

    uint get_dialogue_id(const Wt::WString& dialogue_name) {return dialogue_id_[dialogue_name];}
    std::string get_message_format(const std::string& username, const std::string& message_content, const time_t& time);

    void update_users_list();
    void update_dialogue_list();
    void update_messages(const Wt::WString& username);
    bool create_dialogue(const Wt::WString& username);
    void send_message();

};


#endif //PROJECTKONAMI_CHATWIDGET_HPP
