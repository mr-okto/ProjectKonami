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

#include "ChatWidget.hpp"

ChatWidget::ChatWidget(const Wt::WString& username, uint32_t id, ChatServer& server)
    : Wt::WContainerWidget(),
      server_(server),
      username_(username),
      user_id_(id)

{
    connect();
    create_UI();
}

ChatWidget::ChatWidget(const Wt::WString &username, uint32_t id,
                        const std::optional<std::string> &cookie, ChatServer &server)
        : Wt::WContainerWidget(),
          server_(server),
          username_(username),
          user_id_(id)

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

    messages_ = messagesPtr.get();
    userList_ = userListPtr.get();
    messageEdit_ = messageEditPtr.get();
    sendButton_ = sendButtonPtr.get();
    dialoguesList_ = dialoguesListPtr.get();
    Wt::Core::observing_ptr<Wt::WPushButton> logoutButton = logoutButtonPtr.get();

    sendButton_->clicked().connect(this, [this]() {std::cout << this->current_dialogue_<< std::endl;});
    messageEdit_->setRows(2);
    messageEdit_->setFocus();

    // Display scroll bars if contents overflows
    messages_->setOverflow(Wt::Overflow::Auto);
    userList_->setOverflow(Wt::Overflow::Auto);

    create_layout(std::move(messagesPtr), std::move(userListPtr),
                 std::move(messageEditPtr),
                 std::move(sendButtonPtr), std::move(logoutButtonPtr),
                 std::move(dialoguesListPtr));

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
      sendButton_->clicked().connect(this, &ChatWidget::send);
      sendButton_->clicked().connect(clearInput_);
      sendButton_->clicked().connect((WWidget *)messageEdit_,
				     &WWidget::setFocus);
    }
    messageEdit_->enterPressed().connect(this, &ChatWidget::send);
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
                              std::unique_ptr<Wt::WWidget> logoutButton, std::unique_ptr<Wt::WWidget> chatUserList) {

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

    // 3x3 grid layout
    auto gridLayout = std::make_unique<Wt::WGridLayout>();
    gridLayout->addWidget(create_title("Dialogues"), 0, 0, Wt::AlignmentFlag::Center);
    gridLayout->addWidget(create_title("Messages"), 0, 1, Wt::AlignmentFlag::Center);
    gridLayout->addWidget(create_title("Users"), 0, 2, Wt::AlignmentFlag::Center);

    chatUserList->setStyleClass("chat-users");
    chatUserList->resize(Wt::WLength::Auto, 600);
    gridLayout->addWidget(std::move(chatUserList), 1, 0);

    messages->setStyleClass("chat-msgs");
    gridLayout->addWidget(std::move(messages), 1, 1);

    userList->setStyleClass("chat-users");
    gridLayout->addWidget(std::move(userList), 1, 2);


    auto vLayout = Wt::cpp14::make_unique<Wt::WVBoxLayout>();

    messageEdit->setStyleClass("chat-noedit");
    vLayout->addWidget(std::move(messageEdit));

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

    display = true;

    if (event.type_ == ChatEvent::NEW_DIALOGUE) {
        update_dialogue_list();
    }

    if (event.type_ == ChatEvent::NEW_MSG &&
        dialogue_id_.count(current_dialogue_) && 
        event.dialogue_id_ == dialogue_id_[current_dialogue_]) {

        update_messages(current_dialogue_);
    }
}

void ChatWidget::update_dialogue_list() {
    dialoguesList_->clear();
    auto *l = dialoguesList_->addWidget(std::make_unique<Wt::WSelectionBox>());
    for (const auto& item : server_.get_dialogues(username_)) {
        dialogue_id_[item.username] = item.dialog_id;
        l->addItem(item.username);
    }
    l->activated().connect([this, l] {
        this->update_messages(l->currentText());
        current_dialogue_ = l->currentText();
    });
}

void ChatWidget::update_messages(const Wt::WString& username) {
    messages_->clear();
    for (const auto& item : server_.get_messages(dialogue_id_[username])) {
        Wt::WText *w = messages_->addWidget(Wt::cpp14::make_unique<Wt::WText>());
        if (item.username != username_) {
            // TODO
        } else {
            // TODO
        }
        w->setText(item.username + "   " + item.content);
        w->setInline(false);
    }
}

bool ChatWidget::create_dialogue(const Wt::WString& username) {
    if (server_.create_dialogue(username_, username)) {
        update_dialogue_list();
        return true;
    }
    return false;
}

void ChatWidget::send() {
    if (!messageEdit_->text().empty()) {
        server_.send_msg(dialogue_id_[current_dialogue_], username_, current_dialogue_, messageEdit_->text());
    }
    Wt::WText *w = messages_->addWidget(Wt::cpp14::make_unique<Wt::WText>());
    w->setText(username_ + "   " + messageEdit_->text());
    w->setInline(false);
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
        // TODO dialog window for user
        update_dialogue_list();
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
