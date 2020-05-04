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

    if (display) {

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

void ChatWidget::update_messages(Wt::WString username) {
    messages_->clear();
    for (const auto& item : server_.get_messages(dialogue_id_[username])) {
        Wt::WText *w = messages_->addWidget(Wt::cpp14::make_unique<Wt::WText>());
        if (item.username != username_) {
            // TODO
        } else {
            // TODO
        }
        w->setText(item.username + "   " + item.content);
    }
}

void ChatWidget::update_users_list() {
    if (userList_) {
        userList_->clear();
        dialoguesList_->clear();

        std::set<Wt::WString> users = server_.online_users();

        auto *l = dialoguesList_->addWidget(std::make_unique<Wt::WSelectionBox>());
        for (auto i = users.begin(); i != users.end(); ++i) {
            if (*i != username_) {
                Wt::WText *w = userList_->addWidget(std::make_unique<Wt::WText>(Wt::Utils::htmlEncode(*i)));
                w->setInline(false);
                l->addItem(i->toUTF8());
            }
        }
        Wt::WText* user_box_text = dialoguesList_->addNew<Wt::WText>("");
        // TODO dialog window for user
        l->activated().connect([=] {
            user_box_text->setText(Wt::WString("You selected {1}.").arg(l->currentText()));
            current_dialogue_ = l->currentText();
        });
        update_dialogue_list();
    }
}
