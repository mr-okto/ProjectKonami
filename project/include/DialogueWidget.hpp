#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WImage.h>
#include <Wt/WLink.h>
#include <Wt/WText.h>
#include <Wt/WGroupBox.h>
#include <Wt/WFileResource.h>
#include_next <Wt/WHBoxLayout.h>

const std::string default_ = "./avatars/default-avatar.png";

class DialogueWidget : public Wt::WContainerWidget {
public:
    DialogueWidget(const Wt::WString& dialogue_name, const std::string& path, int unread_message_count) :
            unread_message_count_(unread_message_count),
            dialogue_name_(dialogue_name) {
        auto Layout = std::make_unique<Wt::WHBoxLayout>();

        if (!path.empty())
            avatar_ = Layout->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(std::make_shared<Wt::WFileResource>(path))));
        else
            avatar_ = Layout->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(std::make_shared<Wt::WFileResource>(default_))));

        Layout->addWidget(std::make_unique<Wt::WText>(dialogue_name_), 1);
        unread_widget_ = Layout->addWidget(std::make_unique<Wt::WText>(std::to_string(unread_message_count)), 1);
        /*auto button = Layout->addWidget(std::make_unique<Wt::WPushButton>("View"));
        button->clicked().connect([=] {
            std::cout << "PUSH" << std::endl;
        });*/
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
    Wt::WImage* avatar_;
    Wt::WText* unread_widget_;
};
