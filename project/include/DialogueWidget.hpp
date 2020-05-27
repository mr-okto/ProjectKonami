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
    DialogueWidget(const Wt::WString& dialogue_name,
                   uint dialogue_id,
                   const std::string& path, 
                   int unread_message_count) :
            dialogue_id_(dialogue_id),
            unread_message_count_(unread_message_count),
            dialogue_name_(dialogue_name) {
        auto Layout = std::make_unique<Wt::WHBoxLayout>();
        link_ = Wt::WLink(std::make_shared<Wt::WFileResource>(path));
        avatar_ = Layout->addWidget(std::make_unique<Wt::WImage>(link_));

        auto name = Layout->addWidget(std::make_unique<Wt::WText>(dialogue_name_), 1);
        name->setMargin(10, Wt::Side::Top);
        std::string unread_str = unread_message_count_ ? std::to_string(unread_message_count) : "";
        unread_widget_ = Layout->addWidget(std::make_unique<Wt::WText>(unread_str), 1);

        avatar_->clicked().connect([=] {
            Wt::WDialog dialog(this->dialogue_name_);
            dialog.setClosable(true);
            dialog.rejectWhenEscapePressed(true);
            auto image = dialog.contents()->addWidget(std::make_unique<Wt::WImage>(this->link_));
            image->resize(300, 300);
            dialog.exec();
        });

        Layout->setContentsMargins(0, 0 ,0 ,0);
        setLayout(std::move(Layout));
        setStyleClass("user-widget");
    };

    Wt::WString get_dialogue_name() {return dialogue_name_;};

    uint get_dialogue_id() {return dialogue_id_;};

    void set_unread_message_count(int count) {
        unread_message_count_ = count;
        if (count == 0) {
            unread_widget_->setText("");
        } else {
            unread_widget_->setText(std::to_string(count));
        }
    };

    void set_avatar(const std::string& path) {
        link_ = Wt::WLink(std::make_shared<Wt::WFileResource>(path));
        avatar_->setImageLink(link_);
    }

    int get_unread_message_count() {return unread_message_count_;};

private:
    uint dialogue_id_;
    int unread_message_count_;
    Wt::WString dialogue_name_;
    Wt::WImage* avatar_;
    Wt::WText* unread_widget_;
    Wt::WLink link_;
};
