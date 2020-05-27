#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WImage.h>
#include <Wt/WLink.h>
#include <Wt/WText.h>

typedef unsigned int uint;

class DialogueWidget : public Wt::WContainerWidget {
public:
    DialogueWidget(const Wt::WString& dialogue_name,
                   uint dialogue_id,
                   const std::string& path,
                   int unread_message_count);

    uint get_dialogue_id() const;
    int get_unread_message_count() const;
    const Wt::WString& get_dialogue_name() const;

    void set_avatar(const std::string& path);
    void set_unread_message_count(int count);

private:
    uint dialogue_id_;
    int unread_message_count_;
    Wt::WString dialogue_name_;
    Wt::WImage* avatar_;
    Wt::WText* unread_widget_;
    Wt::WLink link_;
};