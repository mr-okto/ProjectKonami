#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WImage.h>
#include <Wt/WLink.h>
#include <Wt/WText.h>
#include <Wt/WGroupBox.h>
#include_next <Wt/WHBoxLayout.h>

class DialogueWidget : public Wt::WContainerWidget {
public:
    DialogueWidget(const Wt::WString& username, const std::string& path)
            : username_(username),
            avatar_link_(path) {
        auto Layout = std::make_unique<Wt::WHBoxLayout>();
        if (!path.empty())
            avatar_ = Layout->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(avatar_link_)));
        else
            avatar_ = Layout->addWidget(std::make_unique<Wt::WImage>(Wt::WLink("")));

        auto *displayName = Layout->addWidget(std::make_unique<Wt::WText>(username), 1);
        Layout->setContentsMargins(0, 0 ,0 ,0);
        setLayout(std::move(Layout));
        setStyleClass("user-widget");
    };

    Wt::WString get_dialogue_name() {return username_;};

private:
    Wt::WString username_;
    Wt::WLink avatar_link_;

    Wt::WImage* avatar_;
};
