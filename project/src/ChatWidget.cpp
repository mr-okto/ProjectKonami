#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WEnvironment.h>
#include <Wt/WInPlaceEdit.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WSelectionBox.h>
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

ChatWidget::ChatWidget(const Wt::WString& username, uint32_t id, ChatServer& server)
    : Wt::WContainerWidget(),
      server_(server),
      username_(username),
      user_id_(id),
      is_uploaded_(false)

{
    connect();
    create_UI();
}

ChatWidget::ChatWidget(const Wt::WString &username, uint32_t id,
                        const std::optional<std::string> &cookie, ChatServer &server)
        : Wt::WContainerWidget(),
          server_(server),
          username_(username),
          user_id_(id),
          is_uploaded_(false)

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
    std::cout << username_ << " : " << user_id_ << std::endl;

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
        sendButton_->clicked().connect((WWidget *)messageEdit_,
                        &WWidget::setFocus);   
        sendButton_->disable();  
    }
    messageEdit_->enterPressed().connect(this, &ChatWidget::send_message);
    messageEdit_->enterPressed().connect(clearInput_);
    messageEdit_->enterPressed().connect((WWidget *)messageEdit_,
					 &WWidget::setFocus);
    messageEdit_->disable();


    // Prevent the enter from generating a new line, which is its default
    // action
    messageEdit_->enterPressed().preventDefaultAction();

    update_users_list();
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
  * --------------------------------------------
  * | nested WHBoxLayout (vertical stretch=1)  |
  * |                              |           |
  * |  messages                    | userList  |
  * |   (horizontal stretch=1)     |           |
  * |                              |           |
  * --------------------------------------------
  * | message edit area                        |
  * --------------------------------------------
  * |  upload file                             |
  * | send | logout                            |
  * --------------------------------------------
  */

    // 3x3 grid layout
    auto gridLayout = std::make_unique<Wt::WGridLayout>();
    gridLayout->addWidget(create_title("Dialogues"), 0, 0, Wt::AlignmentFlag::Center);
    gridLayout->addWidget(create_title("Messages"), 0, 1, Wt::AlignmentFlag::Center);
    gridLayout->addWidget(create_title("Online Users"), 0, 2, Wt::AlignmentFlag::Center);

    chatUserList->setStyleClass("chat-users");
//    chatUserList->resize(Wt::WLength::Auto, 600);
    gridLayout->addWidget(std::move(chatUserList), 1, 0);

    messages->setStyleClass("chat-msgs");
    gridLayout->addWidget(std::move(messages), 1, 1);

    userList->setStyleClass("chat-users");
    userList->resize(200, 475);
    gridLayout->addWidget(std::move(userList), 1, 2);


    auto vLayout = Wt::cpp14::make_unique<Wt::WVBoxLayout>();

    messageEdit->setStyleClass("chat-noedit");
    vLayout->addWidget(std::move(messageEdit));

    vLayout->addWidget(std::move(fileUploader));

    // Create a horizontal layout for the buttons.
    auto hLayout = Wt::cpp14::make_unique<Wt::WHBoxLayout>();

    // Add button to horizontal layout with stretch = 0
    hLayout->addWidget(std::move(sendButton));

    // Add button to horizontal layout with stretch = 0
    hLayout->addWidget(std::move(logoutButton));

    // Add nested layout to vertical layout with stretch = 0
    vLayout->addLayout(std::move(hLayout), 0, Wt::AlignmentFlag::Left);

    gridLayout->addLayout(std::move(vLayout), 2, 1, 1, 2);

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

    if (event.type() != ChatEvent::Type::NEW_MSG) {
        update_users_list();
    }

    app->triggerUpdate();

    bool display = event.type() != ChatEvent::Type::NEW_MSG
        || !userList_;

    if (event.type_ == ChatEvent::NEW_DIALOGUE) {
        update_dialogue_list();
    } else if (event.type_ == ChatEvent::NEW_MSG) {
        update_dialogue_list();
        if (dialogues_.count(current_dialogue_) &&
                event.dialogue_id_ == dialogues_[current_dialogue_].dialogue_id) {
            print_message(event.message_);
            server_.mark_message_as_read(event.message_);
        }
    } else if(event.type_ == ChatEvent::READ_MESSAGE && 
                dialogues_.count(current_dialogue_) &&
                event.dialogue_id_ == dialogues_[current_dialogue_].dialogue_id) {
        for (int i = messages_->count() - 1; i >= 0; i--) {
            auto widget = messages_->widget(i);
            if (typeid(*widget) == typeid(Wt::WText) && 
                    dynamic_cast<Wt::WText*>(widget)->text() == get_message_format(event.message_)) {
                Wt::WText* w =  messages_->insertBefore(Wt::cpp14::make_unique<Wt::WText>(), widget);
                messages_->removeWidget(widget);
                chat::Message message = event.message_;
                message.is_read = true;
                w->setText(get_message_format(message));
                w->setInline(false);
                break;
            }
        }
    }

}

void ChatWidget::fill_fileuploader() {
    fileUploader_->clear();
    Wt::WFileUpload *fu = fileUploader_->addNew<Wt::WFileUpload>();
    Wt::WText *out = fileUploader_->addNew<Wt::WText>();

    fu->setFilters("video/*|image/*");  // TODO
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

    fileUploaderPtr_ = fu;
}

void ChatWidget::update_dialogue_list() {
    dialoguesList_->clear();
    auto *l = dialoguesList_->addWidget(std::make_unique<Wt::WSelectionBox>());
    for (const auto& dialogue : server_.get_dialogues(username_)) {
        std::string username;
        if (dialogue.first_user.username != username_) {
            username = dialogue.first_user.username;
        } else {
            username = dialogue.second_user.username;
        }
        dialogues_[username] = dialogue;
        l->addItem(username);
    }
    l->activated().connect([=] {
        this->update_messages(l->currentText());
        current_dialogue_ = l->currentText();
        this->sendButton_->enable();
        this->messageEdit_->enable();
    });
}

std::string ChatWidget::get_message_format(const chat::Message& message) {
    struct tm *ts;
    char       buf[80];
    ts = localtime(&message.time);
    strftime(buf, sizeof(buf), "%H:%M", ts);
    if (message.is_read && message.user.username == username_.toUTF8()) {
        return message.user.username + ": " + message.content.message + " " + std::string(buf) + " True";
    } else {
        return message.user.username + ": " + message.content.message + " " + std::string(buf);
    }
}

void ChatWidget::update_messages(const Wt::WString& username) {
    messages_->clear();
    for (const auto& message : server_.get_messages(dialogues_[username].dialogue_id, username_.toUTF8())) {
        print_message(message);
        if (message.user.username != username_ && !message.is_read) {
            server_.mark_message_as_read(message);
        }
    }
}

bool ChatWidget::create_dialogue(const Wt::WString& username) {
    if (server_.create_dialogue(username_, username)) {
        update_dialogue_list();
        return true;
    }
    return false;
}

void ChatWidget::print_message(const chat::Message& message) {
    // Message text
    Wt::WText *w = messages_->addWidget(Wt::cpp14::make_unique<Wt::WText>());
    w->setText(get_message_format(message));
    w->setInline(false);

    // Message media
    if (message.content.type == chat::Content::IMAGE) {
        auto image_resource = std::make_shared<Wt::WFileResource>("image/*", message.content.file_path);
        auto image = messages_->addNew<Wt::WImage>(Wt::WLink(image_resource));
        image->resize(300, 300);
    } else if (message.content.type == chat::Content::VIDEO) {
        auto video_resource = std::make_shared<Wt::WFileResource>("video/*", message.content.file_path);
        auto video = messages_->addNew<Wt::WVideo>();
        video->addSource(Wt::WLink(video_resource)); 
        video->resize(300, 300);
    }

    Wt::WApplication *app = Wt::WApplication::instance();
    app->doJavaScript(messages_->jsRef() + ".scrollTop += "
		       + messages_->jsRef() + ".scrollHeight;");
}

chat::Content::FileType ChatWidget::parse_type(const std::string& filename)  {
    std::string extension = boost::filesystem::extension(filename);
    if (extension == ".mp4") {  // FIXME
        return chat::Content::VIDEO;
    } else {
        return chat::Content::IMAGE;
    }
}

void ChatWidget::send_message() {
    if (!messageEdit_->text().empty() || is_uploaded_) {
        chat::Content content;
        if (is_uploaded_) {
            std::string filename = copy_file(
                fileUploaderPtr_->spoolFileName(), 
                fileUploaderPtr_->clientFileName().toUTF8()
            );
            content = chat::Content(
                parse_type(filename),
                messageEdit_->text().toUTF8(), 
                filename
            );
            is_uploaded_ = false;
            fill_fileuploader();
        } else {
            content = chat::Content(messageEdit_->text().toUTF8());
        }

        chat::Message message(
            dialogues_[current_dialogue_].dialogue_id, 
            chat::User(user_id_, username_.toUTF8()),
            content
        );
          
        chat::User receiver = dialogues_[current_dialogue_].first_user.username != username_ ? 
                                dialogues_[current_dialogue_].first_user : 
                                dialogues_[current_dialogue_].second_user;

        print_message(message);
        server_.send_msg(message, receiver);
    }
}

void ChatWidget::update_users_list() {
    if (userList_) {
        userList_->clear();

        std::set<Wt::WString> users = server_.online_users();
        auto *l = userList_->addWidget(std::make_unique<Wt::WSelectionBox>());
        auto *userListWidget = userList_->addWidget(std::make_unique<Wt::WContainerWidget>());
        userListWidget->setOverflow(Wt::Overflow::Auto);
        for (auto i = users.begin(); i != users.end(); ++i) {
            if (*i != username_) {
                l->addItem(*i);
                userListWidget->addWidget(std::make_unique<UserWidget>(*i, "./avatars/88/img_template_4.jpg"));
            }
        }
        l->activated().connect([this, l] {
            this->create_dialogue(l->currentText());
        });
        update_dialogue_list();
    }
}

std::string ChatWidget::copy_file(const std::string& file_path, const std::string& filename) {
    std::string result_filename = "./photo/" + filename;  // FIXME
    std::ifstream in(file_path, std::ios::binary | std::ios::in);
    std::ofstream out(result_filename, std::ios::binary | std::ios::out);

    char byte = 0;
    while (in.read(&byte, sizeof(char))) {
        out.write(&byte, sizeof(char));
    }
    in.close();
    out.close();

    return result_filename;
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
