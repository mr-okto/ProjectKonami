#pragma once

#include <Wt/WContainerWidget.h>
#include "DialogueService.hpp"

class MessageWidget : public Wt::WContainerWidget {
 public:
    MessageWidget(const chat::Message& message, bool flag);

    void set_message(const chat::Message& message);
    uint get_id() const;
    void set_id(uint id);

 private:
    std::string get_message_format(const chat::Message& message) const;
    
    bool flag_;
    Wt::WText* text_;
    uint id_;
};
