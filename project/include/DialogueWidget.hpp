#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WImage.h>
#include <Wt/WLink.h>
#include <Wt/WText.h>
#include <Wt/WGroupBox.h>
#include <Wt/WFileResource.h>
#include_next <Wt/WHBoxLayout.h>

class DialogueWidget : public Wt::WContainerWidget {
public:
    DialogueWidget(const Wt::WString& dialogue_name, const std::string& path, int unread_message_count) :
            dialogue_name_(dialogue_name),
            avatar_link_(path),
            unread_message_count_(unread_message_count) {
        auto Layout = std::make_unique<Wt::WHBoxLayout>();
        if (!path.empty())
            avatar_ = Layout->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(avatar_link_)));
        else
            avatar_ = Layout->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(std::make_shared<Wt::WFileResource>("image/*", "/Users/aleks/Desktop/tp-firstsem/c++/ProjectKonami/photo/fdffd.jpeg"))));

        auto *displayName = Layout->addWidget(std::make_unique<Wt::WText>(dialogue_name_), 1);
        Layout->setContentsMargins(0, 0 ,0 ,0);
        setLayout(std::move(Layout));
        setStyleClass("user-widget");
    };

    DialogueWidget(const DialogueWidget& widget) :
            dialogue_name_(widget.dialogue_name_),
            avatar_link_(widget.avatar_link_),
            unread_message_count_(widget.unread_message_count_) {
        auto Layout = std::make_unique<Wt::WHBoxLayout>();
        avatar_ = Layout->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(std::make_shared<Wt::WFileResource>("image/*", "/Users/aleks/Desktop/tp-firstsem/c++/ProjectKonami/photo/fdffd.jpeg"))));

        auto *displayName = Layout->addWidget(std::make_unique<Wt::WText>(dialogue_name_), 1);
        Layout->setContentsMargins(0, 0 ,0 ,0);
        setLayout(std::move(Layout));
        setStyleClass("user-widget");
    }

    Wt::WString get_dialogue_name() {return dialogue_name_;};
    void set_unread_message_count(int count) {unread_message_count_ = count;};  // TODO надо сделать чтобы во вью тоже обновлялось
    int get_unread_message_count() {return unread_message_count_;};

private:
    int unread_message_count_;
    Wt::WString dialogue_name_;
    Wt::WLink avatar_link_;
    Wt::WImage* avatar_;
};
