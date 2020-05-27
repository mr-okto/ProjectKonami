#include <Wt/WImage.h>
#include <Wt/WLink.h>
#include <Wt/WText.h>
#include <Wt/WGroupBox.h>
#include <Wt/WFileResource.h>
#include <Wt/WVideo.h>

#include "MessageWidget.hpp"

MessageWidget::MessageWidget(const chat::Message& message) : 
        id_(message.message_id) {

    // Message text
    text_ = this->addWidget(Wt::cpp14::make_unique<Wt::WText>());
    text_->setText(get_message_format(message));
    text_->setInline(false);

    // Message media
    if (message.content.type == chat::Content::IMAGE) {
        auto image_resource = std::make_shared<Wt::WFileResource>("image/*", message.content.file_path);
        auto image = this->addNew<Wt::WImage>(Wt::WLink(image_resource));
        image->resize(300, 300);
    } else if (message.content.type == chat::Content::VIDEO) {
        auto video_resource = std::make_shared<Wt::WFileResource>("video/*", message.content.file_path);
        auto video = this->addNew<Wt::WVideo>();
        video->addSource(Wt::WLink(video_resource)); 
        video->resize(300, 300);
    }
};

void MessageWidget::set_message(const chat::Message& message) {
    text_->setText(get_message_format(message));
}

uint MessageWidget::get_id() const {
    return id_;
}

void MessageWidget::set_id(uint id) {
    id_ = id;
}

std::string MessageWidget::get_message_format(const chat::Message& message) const {
    struct tm *ts;
    char       buf[80];
    ts = localtime(&message.time);
    strftime(buf, sizeof(buf), "%H:%M", ts);

    std::stringstream ss;
    ss << "<p style=\"display: flex; align-items: center; margin-bottom: 0px\">";
            ss << "<span style=\"font-size: large; font-weight: bolder;\">";
                ss << message.user.username;
            ss << "</span>";
            ss << "<span style=\"font-size: 85%; color: Gray; margin-left: 10px\">";
                ss << std::string(buf);
            ss << "</span>";
    ss << "</p>";
    if (message.is_read) {
        ss << message.content.message;
    } else {
        ss << "<font color=\"Gray\">";
            ss << message.content.message;
        ss << "</font>";
    }

    return ss.str();
}