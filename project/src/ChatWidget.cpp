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
#include <Wt/WRasterImage.h>
#include <fstream>


#include "ChatWidget.hpp"

ChatWidget::ChatWidget(const Wt::WString& username, ChatServer& server)
    : Wt::WContainerWidget(),
      server_(server),
      username_(username)

{
    // (TODO) add_session
    connect();
    create_UI();
}

ChatWidget::~ChatWidget() {
    sign_out();
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
    clear();

    auto messagesPtr = Wt::cpp14::make_unique<WContainerWidget>();
    auto userListPtr = Wt::cpp14::make_unique<WContainerWidget>();
    auto dialoguesListPtr = Wt::cpp14::make_unique<Wt::WContainerWidget>();
    auto messageEditPtr = Wt::cpp14::make_unique<Wt::WTextArea>();
    auto sendButtonPtr = Wt::cpp14::make_unique<Wt::WPushButton>("Send");
    auto logoutButtonPtr = Wt::cpp14::make_unique<Wt::WPushButton>("Logout");
    auto fileUploaderPtr = Wt::cpp14::make_unique<Wt::WFileUpload>();

    messages_ = messagesPtr.get();
    userList_ = userListPtr.get();
    messageEdit_ = messageEditPtr.get();
    sendButton_ = sendButtonPtr.get();
    dialoguesList_ = dialoguesListPtr.get();
    fileUploader_ = fileUploaderPtr.get();
    Wt::Core::observing_ptr<Wt::WPushButton> logoutButton = logoutButtonPtr.get();

    fileUploader_->setFileTextSize(50); // Set the maximum file size to 50 kB.
    fileUploader_->setProgressBar(Wt::cpp14::make_unique<Wt::WProgressBar>());

    fileUploader_->changed().connect([=] {
        fileUploader_->upload();
        std::cout << "File upload is changed." << std::endl;;
    });

    // React to a succesfull upload.
    fileUploader_->uploaded().connect([=] {
        this->is_uploaded = true;
    });

    // React to a file upload problem.
    fileUploader_->fileTooLarge().connect([=] {
        std::cout << "File is too large." << std::endl;
    });

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
                     
    }
    messageEdit_->enterPressed().connect(this, &ChatWidget::send_message);
    messageEdit_->enterPressed().connect(clearInput_);
    messageEdit_->enterPressed().connect((WWidget *)messageEdit_,
					 &WWidget::setFocus);

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
  * | WHBoxLayout                              |
  * | send | logout                            |
  * --------------------------------------------
  */
    auto vLayout = Wt::cpp14::make_unique<Wt::WVBoxLayout>();

    // Create a horizontal layout for the messages | userslist.
    auto hLayout = Wt::cpp14::make_unique<Wt::WHBoxLayout>();

    // Choose JavaScript implementation explicitly to avoid log warning (needed for resizable layout)
    hLayout->setPreferredImplementation(Wt::LayoutImplementation::JavaScript);

    chatUserList->setStyleClass("chat-users");
    hLayout->addWidget(std::move(chatUserList));

    // Add widget to horizontal layout with stretch = 1
    messages->setStyleClass("chat-msgs");
    hLayout->addWidget(std::move(messages), 1);

    // Add another widget to horizontal layout with stretch = 0
    userList->setStyleClass("chat-users");
    hLayout->addWidget(std::move(userList));

    hLayout->setResizable(0, true);

    // Add nested layout to vertical layout with stretch = 1
    vLayout->addLayout(std::move(hLayout), 1);

    // Add widget to vertical layout with stretch = 0
    messageEdit->setStyleClass("chat-noedit");
    vLayout->addWidget(std::move(messageEdit));

    vLayout->addWidget(std::move(fileUploader));

    // Create a horizontal layout for the buttons.
    hLayout = Wt::cpp14::make_unique<Wt::WHBoxLayout>();

    // Add button to horizontal layout with stretch = 0
    hLayout->addWidget(std::move(sendButton));

    // Add button to horizontal layout with stretch = 0
    hLayout->addWidget(std::move(logoutButton));

    // Add nested layout to vertical layout with stretch = 0
    vLayout->addLayout(std::move(hLayout), 0, Wt::AlignmentFlag::Left);

    this->setLayout(std::move(vLayout));
}

void ChatWidget::sign_out() {
    if (server_.sign_out(username_)) {
        disconnect();
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
    }

    if (event.type_ == ChatEvent::NEW_MSG ) {
        update_dialogue_list();
        if (dialogue_id_.count(current_dialogue_) &&
                event.dialogue_id_ == dialogue_id_[current_dialogue_].dialogue_id) {
            update_messages(current_dialogue_);
        }
    }
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
        dialogue_id_[username] = dialogue;
        l->addItem(username);
    }
    l->activated().connect([this, l] {
        this->update_messages(l->currentText());
        current_dialogue_ = l->currentText();
    });
}

std::string ChatWidget::get_message_format(const std::string& username, const std::string& message_content, const time_t& time) {
    struct tm *ts;
    char       buf[80];
    ts = localtime(&time);
    strftime(buf, sizeof(buf), "%H:%M", ts);
    return username + ": " + message_content + " " + std::string(buf);
}

void ChatWidget::update_messages(const Wt::WString& username) {
    messages_->clear();
    for (const auto& message : server_.get_messages(dialogue_id_[username].dialogue_id)) {
        Wt::WText *w = messages_->addWidget(Wt::cpp14::make_unique<Wt::WText>());
        if (message.user.username != username_) {
            // TODO
        } else {
            // TODO
        }
        w->setText(get_message_format(message.user.username, message.content.message, message.time));
        w->setInline(false);
        if (message.content.type == chat::Content::IMAGE) {
            auto imageFile = std::make_shared<Wt::WFileResource>("image", message.content.file_path);
            //Wt::WImage *image = messages_->addNew<Wt::WImage>(Wt::WLink(imageFile));
            messages_->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(imageFile)));
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

void ChatWidget::send_message() {
    if (!messageEdit_->text().empty()) {
        chat::Message message;
        message.dialogue_id = dialogue_id_[current_dialogue_].dialogue_id;
        message.user = {user_id_, username_.toUTF8()};
        message.time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        if (is_uploaded) {
            message.content = {chat::Content::IMAGE, // FIXME
                               messageEdit_->text().toUTF8(), 
                               fileUploader_->spoolFileName()};
        } else {
            message.content = {chat::Content::WITHOUTFILE, 
                               messageEdit_->text().toUTF8(), 
                               "NULL"};
        }
        is_uploaded = false;
                                 
        chat::User receiver;
        if (dialogue_id_[current_dialogue_].first_user.username != username_) {
            receiver = dialogue_id_[current_dialogue_].first_user;
        } else {
            receiver = dialogue_id_[current_dialogue_].second_user;
        }

        server_.send_msg(message, receiver);

        // Message text
        Wt::WText *w = messages_->addWidget(Wt::cpp14::make_unique<Wt::WText>());
        w->setText(get_message_format(message.user.username, message.content.message, message.time));
        w->setInline(false);

        // Media
        if (message.content.type == chat::Content::IMAGE) {  // TODO add more...
            Wt::WString filename = fileUploader_->clientFileName();
            std::stringstream ss;
            bool flag = false;
            for (const auto& symbol : filename.toUTF8()) {
                if (flag) {
                    ss << symbol;
                }
                if (symbol == '.') {
                    flag = true;
                }
            }
            auto imageFile = std::make_shared<Wt::WFileResource>("image/" + ss.str(), fileUploader_->spoolFileName());
            imageFile->suggestFileName(filename);
            messages_->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(imageFile)));
        }
    }
}

void ChatWidget::update_users_list() {
    if (userList_) {
        userList_->clear();

        std::set<Wt::WString> users = server_.online_users();
        auto *l = userList_->addWidget(std::make_unique<Wt::WSelectionBox>());
        for (auto i = users.begin(); i != users.end(); ++i) {
            if (*i != username_) {
                l->addItem(*i);
            }
        }
        l->activated().connect([this, l] {
            this->create_dialogue(l->currentText());
        });
        update_dialogue_list();
    }
}
