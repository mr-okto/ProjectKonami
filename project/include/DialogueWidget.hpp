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
        unread_widget_ = Layout->addWidget(std::make_unique<Wt::WText>(std::to_string(unread_message_count)), 1);
        Layout->setContentsMargins(0, 0 ,0 ,0);
        setLayout(std::move(Layout));
        setStyleClass("user-widget");
    };

    Wt::WString get_dialogue_name() {return dialogue_name_;};
    void set_unread_message_count(int count) {
        unread_message_count_ = count;
        if (count == 0) {
            unread_widget_->setText("");
        } else {
            unread_widget_->setText(std::to_string(count));
        }
    };
    int get_unread_message_count() {return unread_message_count_;};

private:
    int unread_message_count_;
    Wt::WString dialogue_name_;
    Wt::WLink avatar_link_;
    Wt::WImage* avatar_;
    Wt::WText* unread_widget_;
};