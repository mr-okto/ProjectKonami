#ifndef PROJECTKONAMI_EDITPROFILEFORM_HPP
#define PROJECTKONAMI_EDITPROFILEFORM_HPP

#include <Wt/WContainerWidget.h>
#include <Wt/WString.h>
#include <Wt/WText.h>
#include <Wt/WSignal.h>
#include <Wt/WFileUpload.h>

#include "ChatServer.hpp"

class EditProfileForm : public Wt::WContainerWidget {
public:
    explicit EditProfileForm(const Wt::WString& username, uint32_t id, ChatServer& server, Wt::WText& status);

    Wt::Signal <Wt::WString>& updated_username() { return updated_username_; }

    enum ErrorType {
        None,
        ShortPassword,
        WeakPassword,
        UsernameUnchanged,
        PasswordMismatch,
        ImgTooLarge
    };

    ErrorType error() const { return error_; }
private:
    void create_UI();
    void create_file_uploader();

    void update_username();
    Wt::Signal <Wt::WString> updated_username_;

    void change_avatar();

    Wt::WContainerWidget * file_upload_container_;
    Wt::WFileUpload *file_upload_;

    ChatServer& server_;

    Wt::WText& status_;
    bool is_valid_;
    ErrorType error_;

    Wt::WString current_username_;
    uint32_t id_;

    Wt::WLineEdit *username_edit_;

    std::string copy_temp_img_to_avatar_folder(
            const std::string& tmp_file_path, const std::string& client_filename);

};


#endif //PROJECTKONAMI_EDITPROFILEFORM_HPP
