#ifndef PROJECTKONAMI_CHATWIDGET_HPP
#define PROJECTKONAMI_CHATWIDGET_HPP

#include <Wt/WContainerWidget.h>
#include <Wt/WMessageBox.h>
#include <Wt/WLink.h>

#include "DialogueWidget.hpp"
#include "MessageWidget.hpp"

#include "ChatServer.hpp"

class ChatWidget : public Wt::WContainerWidget, public Client {
public:
    ChatWidget(const Wt::WString& username, uint32_t id, ChatServer& server);
    ChatWidget(const Wt::WString& username, uint32_t id,
               const std::optional<std::string>& cookie, ChatServer& server);

    void connect() override;
    void disconnect() override;
    void sign_out() override;
    Wt::Signal<Wt::WString>& logout_signal() {return logout_signal_;}

    std::string username() override {return username_.toUTF8();}

    ~ChatWidget();

private:
    ChatServer& server_;
    Wt::WString username_;
    Wt::WLink   avatar_link_;
    uint32_t    user_id_;
    bool        is_uploaded_;
    uint current_dialogue_id_;
    std::map<uint, chat::Dialogue> dialogues_;
    std::map<uint, DialogueWidget*> dialogues_widgets_;

    Wt::Signal<Wt::WString>                       logout_signal_;
    Wt::JSlot                                     clearInput_;
    Wt::WContainerWidget*                         messages_;
    Wt::WTextArea*                                messageEdit_;
    Wt::Core::observing_ptr<Wt::WPushButton>      sendButton_;
    Wt::Core::observing_ptr<Wt::WContainerWidget> userList_;
    Wt::Core::observing_ptr<Wt::WContainerWidget> dialoguesList_;
    Wt::Core::observing_ptr<Wt::WContainerWidget> fileUploader_;
    Wt::Core::observing_ptr<Wt::WFileUpload>      fileUploaderPtr_;

    void fill_fileuploader();
    void create_UI();
    void create_layout(std::unique_ptr<Wt::WWidget> messages,
                    std::unique_ptr<Wt::WWidget> userList,
                    std::unique_ptr<Wt::WWidget> messageEdit,
                    std::unique_ptr<Wt::WWidget> sendButton,
                    std::unique_ptr<Wt::WWidget> logoutButton,
                    std::unique_ptr<Wt::WWidget> chatUserList,
                    std::unique_ptr<Wt::WWidget> fileUploader);
    std::unique_ptr<Wt::WText> create_title(const Wt::WString& title);

    void process_chat_event(const ChatEvent& event);

    bool change_photo_if_access_level_changed(DialogueWidget* dialogue);

    void update_users_list();
    void update_dialogue_list();
    void set_dialogue_top(DialogueWidget* dialogue);
    void update_messages(uint dialogue_id) ;
    MessageWidget* print_message(const chat::Message& message);
    chat::Dialogue create_dialogue(const Wt::WString& username);
    void send_message();

    void close_same_session();
};


#endif //PROJECTKONAMI_CHATWIDGET_HPP
