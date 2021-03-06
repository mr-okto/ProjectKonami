#ifndef PROJECTKONAMI_USERWIDGET_HPP
#define PROJECTKONAMI_USERWIDGET_HPP

#include <Wt/WContainerWidget.h>
#include <Wt/WImage.h>
#include <Wt/WLink.h>
#include <Wt/WText.h>
#include <Wt/WGroupBox.h>
#include <Wt/WFileResource.h>
#include <Wt/WHBoxLayout.h>

class UserWidget : public Wt::WContainerWidget {
public:
    UserWidget(const Wt::WString& username, const std::string& path)
            : username_(username) {
        auto Layout = std::make_unique<Wt::WHBoxLayout>();

        std::cout << " PATH (USERWIDGET) " << path << std::endl;

        avatar_ = Layout->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(std::make_shared<Wt::WFileResource>(path))));

    //    avatar_->setMargin(30, Wt::WFlags<Wt::Side>::enum_type::Right);
        auto name = Layout->addWidget(std::make_unique<Wt::WText>(username), 1);
        name->setMargin(10, Wt::Side::Top);
        Layout->setContentsMargins(0, 0 ,0 ,0);
        setLayout(std::move(Layout));
        setStyleClass("user-widget");
    };

    Wt::WString get_username() {return username_;};

private:
    Wt::WString username_;
    Wt::WImage* avatar_;
};


#endif //PROJECTKONAMI_USERWIDGET_HPP
