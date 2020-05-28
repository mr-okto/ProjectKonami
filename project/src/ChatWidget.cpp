#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WEnvironment.h>
#include <Wt/WInPlaceEdit.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/Utils.h>
#include <Wt/WFileUpload.h>
#include <Wt/WFileResource.h>
#include <Wt/WProgressBar.h>
#include <Wt/WVideo.h>
#include <boost/filesystem.hpp>
#include <fstream>

#include "ChatWidget.hpp"
#include "UserWidget.hpp"

const std::string MEDIA_ROOT = "./media";

static chat::Content::FileType parse_type(const std::string& extension) {
    if (extension == ".jpeg" || extension == ".png" || extension == ".jpg") {  // TODO
        return chat::Content::IMAGE;
    } else if (extension == ".mp4") {
        return chat::Content::VIDEO;
    } else {
        return chat::Content::VIDEO;
    }
}

static int get_access_level(uint message_count) {
    if (message_count < 5) {
        return 5;
    } else if (message_count < 10) {
        return 4;
    } else if (message_count < 15) {
        return 3;
    } else if (message_count < 20) {
        return 2;
    } else {
        return 1;
    }
}

static std::pair<std::string, chat::Content::FileType> save_file(
            const std::string& file_path, 
            const std::string& filename,
            uint user_id) {
    std::string extension = boost::filesystem::extension(filename);
    chat::Content::FileType type = parse_type(extension);

    std::stringstream result_filename;
    result_filename << MEDIA_ROOT;
    if (type == chat::Content::IMAGE) {
        result_filename << "/image/";
    } else if (type == chat::Content::VIDEO) {
        result_filename << "/video/";
    } else {
        result_filename << "/other/";
    }
    result_filename << user_id << "_";
    result_filename << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    result_filename << extension;

    std::ifstream in(file_path, std::ios::binary | std::ios::in);
    std::ofstream out(result_filename.str(), std::ios::binary | std::ios::out);

    char byte = 0;
    while (in.read(&byte, sizeof(char))) {
        out.write(&byte, sizeof(char));
    }
    in.close();
    out.close();

    return std::make_pair(result_filename.str(), type);
}

ChatWidget::ChatWidget(const Wt::WString& username, uint32_t id, ChatServer& server)
    : Wt::WContainerWidget(),
      server_(server),
      username_(username),
      avatar_link_(
              std::make_shared<Wt::WFileResource>(
                      server_.get_user_picture(username_, 1))),
      user_id_(id),
      is_uploaded_(false),
      current_dialogue_id_(INT32_MAX)

{
    connect();
    create_UI();
}

ChatWidget::ChatWidget(const Wt::WString &username, uint32_t id,
                        const std::optional<std::string> &cookie, ChatServer &server)
        : Wt::WContainerWidget(),
          server_(server),
          username_(username),
          avatar_link_(
                  std::make_shared<Wt::WFileResource>(
                          server_.get_user_picture(username_, 1))),
          user_id_(id),
          is_uploaded_(false),
          current_dialogue_id_(INT32_MAX)

{
    //(TODO) add graceful shutdown to session-cookie by timeout (implement singletone scheduler)
    close_same_session();
    connect();
    server_.set_cookie(username_.toUTF8(), cookie.value());
    create_UI();
}

ChatWidget::~ChatWidget() {
    //(TODO) split 'sign out' by pressing the button (delete session cookie) and 'sign out' by closing the tab (not delete session cookie)
    auto cookie = Wt::WApplication::instance()->environment().getCookie("username");
    if (!cookie || cookie->empty()) {
        std::cout << "SIGN OUT DISCONECT: " <<  std::endl;
        sign_out();
    } else {
        std::string username = server_.check_cookie(*cookie);
        if (!username.empty() && username == username_) {
            std::cout << "CLOSE WINDOW DISCONECT: " << *cookie << std::endl;
            server_.weak_sign_out(this, username);
        } else {
            sign_out();
        }
    }
}

void ChatWidget::connect() {
    if (server_.connect
        (this, std::bind(&ChatWidget::process_chat_event, this, std::placeholders::_1))) {
        Wt::WApplication::instance()->enableUpdates(true);
    }
}

void ChatWidget::disconnect() {
    if (server_.disconnect(this)) {
        Wt::WApplication::instance()->enableUpdates(true);
    }
}

void ChatWidget::create_UI() {
    std::cout << username_ << " : " << user_id_ << " : " << avatar_link_.url() << std::endl;

    clear();

    auto messagesPtr = Wt::cpp14::make_unique<WContainerWidget>();
    auto userListPtr = Wt::cpp14::make_unique<WContainerWidget>();
    auto dialoguesListPtr = Wt::cpp14::make_unique<Wt::WContainerWidget>();
    auto messageEditPtr = Wt::cpp14::make_unique<Wt::WTextArea>();
    auto sendButtonPtr = Wt::cpp14::make_unique<Wt::WPushButton>("Send");
    auto logoutButtonPtr = Wt::cpp14::make_unique<Wt::WPushButton>("Logout");
    auto fileUploaderPtr = Wt::cpp14::make_unique<Wt::WContainerWidget>();

    messages_ = messagesPtr.get();
    userList_ = userListPtr.get();
    messageEdit_ = messageEditPtr.get();
    sendButton_ = sendButtonPtr.get();
    dialoguesList_ = dialoguesListPtr.get();
    fileUploader_ = fileUploaderPtr.get();
    Wt::Core::observing_ptr<Wt::WPushButton> logoutButton = logoutButtonPtr.get();

    fill_fileuploader();

    messageEdit_->setRows(2);
    messageEdit_->setFocus();

    // Display scroll bars if contents overflows
    messages_->setOverflow(Wt::Overflow::Auto);
    userList_->setOverflow(Wt::Overflow::Auto);
    dialoguesList_->setOverflow(Wt::Overflow::Auto);

    create_layout(std::move(messagesPtr), std::move(userListPtr),
                 std::move(messageEditPtr),
                 std::move(sendButtonPtr), std::move(logoutButtonPtr),
                 std::move(dialoguesListPtr),
                 std::move(fileUploaderPtr));

    logoutButton->clicked().connect([this]() {
        Wt::WApplication::instance()->setCookie("username", std::string{}, 0);
        Wt::WApplication::instance()->removeCookie("username");
        sign_out();
    });

    /*
     * Connect event handlers:
     *  - click on button
     *  - enter in text area
     *
     * We will clear the input field using a small custom client-side
     * JavaScript invocation.
     */

    // Create a JavaScript 'slot' (JSlot). The JavaScript slot always takes
    // 2 arguments: the originator of the event (in our case the
    // button or text area), and the JavaScript event object.
    clearInput_.setJavaScript
            ("function(o, e) { setTimeout(function() {"
             "" + messageEdit_->jsRef() + ".value='';"
                                          "}, 0); }");

    /*
     * Set the connection monitor
     *
     * The connection monitor is a javascript monitor that will
     * nootify the given object by calling the onChange method to
     * inform of connection change (use of websockets, connection
     * online/offline) Here we just disable the TextEdit when we are
     * offline and enable it once we're back online
     */
    Wt::WApplication::instance()->setConnectionMonitor(
            "window.monitor={ "
            "'onChange':function(type, newV) {"
            "var connected = window.monitor.status.connectionStatus != 0;"
            "if(connected) {"
            + messageEdit_->jsRef() + ".disabled=false;"
            + messageEdit_->jsRef() + ".placeholder='';"
                                      "} else { "
            + messageEdit_->jsRef() + ".disabled=true;"
            + messageEdit_->jsRef() + ".placeholder='connection lost';"
                                      "}"
                                      "}"
                                      "}"
    );

    if (sendButton_) {
        sendButton_->clicked().connect(this, &ChatWidget::send_message);
        sendButton_->clicked().connect(clearInput_);
        sendButton_->clicked().connect((WWidget *)messageEdit_, &WWidget::setFocus);   
        sendButton_->disable();  
    }
    messageEdit_->enterPressed().connect(this, &ChatWidget::send_message);
    messageEdit_->enterPressed().connect(clearInput_);
    messageEdit_->enterPressed().connect((WWidget *)messageEdit_, &WWidget::setFocus);
    messageEdit_->disable();


    // Prevent the enter from generating a new line, which is its default
    // action
    messageEdit_->enterPressed().preventDefaultAction();

    update_users_list();
    update_dialogue_list();
}

void ChatWidget::create_layout(std::unique_ptr<Wt::WWidget> messages, std::unique_ptr<Wt::WWidget> userList,
                              std::unique_ptr<Wt::WWidget> messageEdit, std::unique_ptr<Wt::WWidget> sendButton,
                              std::unique_ptr<Wt::WWidget> logoutButton, std::unique_ptr<Wt::WWidget> chatUserList,
                              std::unique_ptr<Wt::WWidget> fileUploader) {

 /*
  * Create a vertical layout, which will hold 3 rows,
  * organized like this:
  *
  * WVBoxLayout
  * ------------------------------------------------------
  *           | nested WHBoxLayout (vertical stretch=1)  |
  *           |                              |           |
  * Dialogues |         messages             | userList  |
  *           |   (horizontal stretch=1)     |           |
  *           |                              |           |
  *           |-------------------------------------------
  *           | message edit area                        |
  *           | ------------------------------------------
  *           |  upload file                             |
  *           | send               edit profile | Logout |
  * ------------------------------------------------------
  */

    // 3x3 grid layout
    auto gridLayout = std::make_unique<Wt::WGridLayout>();
    gridLayout->addWidget(create_title("Dialogues"), 0, 0, Wt::AlignmentFlag::Center);
    gridLayout->addWidget(create_title("Messages"), 0, 1, Wt::AlignmentFlag::Center);
    gridLayout->addWidget(create_title("Online Users"), 0, 2, Wt::AlignmentFlag::Center);

    chatUserList->setStyleClass("chat-users");
    chatUserList->resize(200, 475);
    gridLayout->addWidget(std::move(chatUserList), 1, 0);

    messages->setStyleClass("chat-msgs");
    gridLayout->addWidget(std::move(messages), 1, 1);

    userList->setStyleClass("chat-users");
    userList->resize(200, 475);
    gridLayout->addWidget(std::move(userList), 1, 2);


    auto vLayout = std::make_unique<Wt::WVBoxLayout>();

    messageEdit->setStyleClass("chat-noedit");
    vLayout->addWidget(std::move(messageEdit));

    vLayout->addWidget(std::move(fileUploader));

    // Create a horizontal layout for the buttons.
    auto hLayout = std::make_unique<Wt::WHBoxLayout>();

    // Add button to horizontal layout with stretch = 0
    hLayout->addWidget(std::move(sendButton));

    auto btnsLayout = std::make_unique<Wt::WHBoxLayout>();
    btnsLayout->addWidget(std::make_unique<Wt::WPushButton>("Edit Profile"));
    btnsLayout->addWidget(std::move(logoutButton));

    // Add button to horizontal layout with stretch = 1
    hLayout->addLayout(std::move(btnsLayout), 1, Wt::AlignmentFlag::Right);
//    btn->setMargin(400, Wt::Side::Left);

    // Add nested layout to vertical layout with stretch = 0
    vLayout->addLayout(std::move(hLayout));
    gridLayout->addLayout(std::move(vLayout), 2, 1, 1, 2);

    auto profileLayout = std::make_unique<Wt::WHBoxLayout>();
    auto profile = std::make_unique<Wt::WContainerWidget>();
    Wt::WImage* profilePictue = profile->addWidget(std::make_unique<Wt::WImage>(avatar_link_));
    profile->addWidget(std::make_unique<Wt::WText>(username_));
    profile->setStyleClass("profile-widget");
    profile->setMaximumSize(200, Wt::WLength::Auto);
    profileLayout->addWidget(std::move(profile));
    gridLayout->addLayout(std::move(profileLayout), 2, 0);

    gridLayout->setRowStretch(1, 1);
    gridLayout->setColumnStretch(1, 1);
    this->setLayout(std::move(gridLayout));
}

void ChatWidget::sign_out() {
    if (server_.sign_out(username_)) {
        disconnect();
        logout_signal_.emit(Wt::WString());
    }
}

void ChatWidget::process_chat_event(const ChatEvent& event) {
    Wt::WApplication *app = Wt::WApplication::instance();

    if (event.type() != ChatEvent::Type::NEW_MSG && 
            event.type() != ChatEvent::READ_MESSAGE) {
        update_users_list();
    }

    app->triggerUpdate();

    if (event.type_ == ChatEvent::NEW_DIALOGUE) {
        update_dialogue_list();

    } else if (event.type_ == ChatEvent::NEW_MSG) {
        DialogueWidget* dialogue = dialogues_widgets_[event.message_.dialogue_id];
    
        change_photo_if_access_level_changed(dialogue);
        set_dialogue_top(dialogue);
        dialogues_[event.message_.dialogue_id].messages_count++;
    
        if (event.dialogue_id_ == current_dialogue_id_) {
            chat::Message message = event.message_;
            message.is_read = true;
            print_message(message);
            server_.mark_message_as_read(event.message_);
            dialogue->set_unread_message_count(0);
        } else {
            dialogue->set_unread_message_count(dialogue->get_unread_message_count() + 1);
        }

    } else if (event.type_ == ChatEvent::READ_MESSAGE && 
                event.dialogue_id_ == current_dialogue_id_) {
        for (int i = messages_->count() - 1; i >= 0; i--) {
            WWidget* widget = messages_->widget(i);
            if (typeid(*widget) == typeid(MessageWidget) && 
                    dynamic_cast<MessageWidget*>(widget)->get_id() == event.message_.message_id) {
                chat::Message message = event.message_;
                message.is_read = true;
                dynamic_cast<MessageWidget*>(widget)->set_message(message);
                break;
            }
        }
    } else if (event.type_ == ChatEvent::UPDATE_AVATAR) {
        /*if (dialogues_widgets_.count(event.username_)) {
            dialogues_widgets_[event.username_]->set_avatar(event.data_.toUTF8());
        }*/
    }
}

bool ChatWidget::change_photo_if_access_level_changed(DialogueWidget* dialogue) {
    uint msg_count = dialogues_[dialogue->get_dialogue_id()].messages_count;
 
    if (get_access_level(msg_count) != get_access_level(msg_count + 1)) {
        Wt::WString dialogue_name = dialogue->get_dialogue_name();
        std::string avatar_path = server_.get_user_picture(
            dialogue_name, get_access_level(msg_count + 1)
        );
        dialogue->set_avatar(avatar_path);
        return true;
    } else {
        return false;
    }
}

void ChatWidget::fill_fileuploader() {
    fileUploader_->clear();
    Wt::WFileUpload *fu = fileUploader_->addNew<Wt::WFileUpload>();
    Wt::WText *out = fileUploader_->addNew<Wt::WText>();

    fu->setFilters("image/jpeg,image/png,image/jpeg,video/mp4");  // TODO
    fu->setFileTextSize(10000); // Set the maximum file size to 10000 kB.
    fu->setProgressBar(Wt::cpp14::make_unique<Wt::WProgressBar>());

    // File upload automatically
    fu->changed().connect([=] {
        fu->upload();
        this->sendButton_->disable();
    });

    // React to a succesfull upload.
    fu->uploaded().connect([=] {
        this->is_uploaded_ = true;
        this->sendButton_->enable();
        out->setText("File upload is finished.");
    });

    // React to a file upload problem.
    fu->fileTooLarge().connect([=] {
        out->setText("File is too large.");
        this->sendButton_->enable();
        this->fill_fileuploader();
    });

    if (current_dialogue_id_ == INT32_MAX) {
        fu->disable();
    }

    fileUploaderPtr_ = fu;
}

void ChatWidget::set_dialogue_top(DialogueWidget* dialogue) {
    dialoguesList_->insertWidget(0, dialoguesList_->removeWidget(dialogue));
}

void ChatWidget::update_dialogue_list() {
    dialoguesList_->clear();
    auto avatars = server_.avatar_map();
    for (const auto& dialogue : server_.get_dialogues(user_id_)) {
        std::string username = dialogue.first_user.username != username_ ? 
                               dialogue.first_user.username :
                               dialogue.second_user.username;
        int unread_messages_count = server_.get_unread_messages_count(dialogue.dialogue_id, user_id_);
        auto w = dialoguesList_->addWidget(
            std::make_unique<DialogueWidget>(
                username,
                dialogue.dialogue_id,
                server_.get_user_picture(username, get_access_level(dialogue.messages_count)),
                unread_messages_count
            )
        );
        w->clicked().connect([=] {
            this->update_messages(w->get_dialogue_id());
            this->current_dialogue_id_ = w->get_dialogue_id();
            this->fileUploaderPtr_->enable();
            this->sendButton_->enable();
            this->messageEdit_->enable();
        });
        dialogues_[dialogue.dialogue_id] = dialogue;
        dialogues_widgets_[dialogue.dialogue_id] = w;
    }
}


void ChatWidget::update_messages(uint dialogue_id) {
    messages_->clear();
    for (auto& message : server_.get_messages(dialogue_id)) {
        if (message.user.username != username_ && !message.is_read) {
            server_.mark_message_as_read(message);
            message.is_read = true;
        }
        print_message(message);
        dialogues_widgets_[message.dialogue_id]->set_unread_message_count(0);
    }

}

chat::Dialogue ChatWidget::create_dialogue(const Wt::WString& username) {
    std::pair<chat::Dialogue, bool> dialogue_iscreate = server_.create_dialogue(user_id_, username);
    if (dialogue_iscreate.second) {
        update_dialogue_list();
    }
    return dialogue_iscreate.first;
}

 MessageWidget* ChatWidget::print_message(const chat::Message& message) {
    bool flag = message.user.username == username_;
    auto message_widget = messages_->addWidget(std::make_unique<MessageWidget>(message, flag));
    // JS trick for page scroll
    Wt::WApplication *app = Wt::WApplication::instance();
    app->doJavaScript(messages_->jsRef() + ".scrollTop += "
		       + messages_->jsRef() + ".scrollHeight;");
    
    return message_widget;
}

void ChatWidget::send_message() {
    if (!messageEdit_->text().empty() || is_uploaded_) {
        chat::Content content;
        if (is_uploaded_) {
            std::pair<std::string, chat::Content::FileType> filepath_type = save_file(
                fileUploaderPtr_->spoolFileName(), 
                fileUploaderPtr_->clientFileName().toUTF8(),
                user_id_
            );
            content = chat::Content(
                filepath_type.second,
                messageEdit_->text().toUTF8(), 
                filepath_type.first
            );
            is_uploaded_ = false;
            fill_fileuploader();
        } else {
            content = chat::Content(messageEdit_->text().toUTF8());
        }

        chat::Message message(
            current_dialogue_id_, 
            chat::User(user_id_, username_.toUTF8()),
            content
        );
          
        chat::User receiver = dialogues_[current_dialogue_id_].first_user.username != username_ ?  // If
                              dialogues_[current_dialogue_id_].first_user :   // Else 
                              dialogues_[current_dialogue_id_].second_user;

        DialogueWidget* widget = dialogues_widgets_[current_dialogue_id_];
        set_dialogue_top(widget);
        widget->set_unread_message_count(0);

        MessageWidget* message_widget = print_message(message);
        server_.send_msg(message, receiver);
        message_widget->set_id(message.message_id);
    }
}

void ChatWidget::update_users_list() {
    if (userList_) {
        userList_->clear();
        std::map<Wt::WString, Wt::WString> avatars = server_.avatar_map();
        for (const auto& user : server_.online_users()) {
            if (user != username_) {
//                auto w = userList_->addWidget(std::make_unique<UserWidget>(user, avatars[user].toUTF8()));
                auto w = userList_->addWidget(std::make_unique<UserWidget>(user, server_.get_user_picture(user, 5)));
                w->clicked().connect([=] {
                    chat::Dialogue d = this->create_dialogue(w->get_username());
                    this->update_messages(d.dialogue_id);
                    this->current_dialogue_id_ = d.dialogue_id;
                    this->fileUploaderPtr_->enable();
                    this->sendButton_->enable();
                    this->messageEdit_->enable();
                });
            }
        }
    }
}

std::unique_ptr<Wt::WText> ChatWidget::create_title(const Wt::WString& title) {
    auto text = std::make_unique<Wt::WText>(title);
    text->setInline(false);
    text->setStyleClass("chat-title");
    return text;
}

void ChatWidget::close_same_session() {
    server_.close_same_session(username_);
}
