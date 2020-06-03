#include <Wt/WVBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WProgressBar.h>
#include <filesystem>

#include "EditProfileForm.hpp"

EditProfileForm::EditProfileForm(const Wt::WString& username, uint32_t id, ChatServer& server, Wt::WText& status)
    :Wt::WContainerWidget(),
     server_(server),
     status_(status),
     current_username_(username),
     id_(id),
     is_valid_(false),
     error_(ErrorType::None)
{
    setFloatSide(Wt::Side::CenterX);
    create_UI();
}

void EditProfileForm::create_UI() {
    auto vLayout = setLayout(std::make_unique<Wt::WVBoxLayout>());
    vLayout->setContentsMargins(10,10,10,10);

    auto line = vLayout->addWidget(std::make_unique<Wt::WContainerWidget>(),0,Wt::AlignmentFlag::Middle);
    line->addWidget(std::make_unique<Wt::WText>("Upload photo: "));
    file_upload_container_ = line->addWidget(std::make_unique<Wt::WContainerWidget>());
    create_file_uploader();

    Wt::WPushButton *changeAvatarbtn = line->addWidget(std::make_unique<Wt::WPushButton>("Change avatar"));
    changeAvatarbtn->setMargin(15, Wt::Side::Top);
    changeAvatarbtn->clicked().connect([this]() {
        change_avatar();
    });

//    auto label_ = std::make_unique<Wt::WLabel>("Change username: ");
//    auto label = label_.get();
//    line->addWidget(std::move(label_));
//    username_edit_ = line->addWidget(std::make_unique<Wt::WLineEdit>());
//    username_edit_->setFloatSide(Wt::Side::Right);
//    username_edit_->setMinimumSize(150, 16);
//    username_edit_->setMaximumSize(150, 16);
//    username_edit_->setText(current_username_);
//    label->setBuddy(username_edit_);
//    Wt::WPushButton *updateUsernameBtn = line->addWidget(std::make_unique<Wt::WPushButton>("Update username"));
//    updateUsernameBtn->setInline(false);
//    updateUsernameBtn->clicked().connect([this]() {
//        update_username();
//    });
}

void EditProfileForm::update_username() {
    auto newUsername = username_edit_->text();
    if (current_username_ == newUsername) {
        error_ = ErrorType::UsernameUnchanged;
        is_valid_ = false;
        status_.setText("Login remained unchanged");
    } else {
        server_.update_username(id_, current_username_, newUsername);
        updated_username_.emit(newUsername);
    }
}

void EditProfileForm::create_file_uploader() {
    file_upload_container_->clear();
    file_upload_ = file_upload_container_->addWidget(std::make_unique<Wt::WFileUpload>());
    file_upload_->setInline(true);
    file_upload_->setFileTextSize(100);
    file_upload_->setProgressBar(std::make_unique<Wt::WProgressBar>());
    file_upload_->setFilters("image/*");

    file_upload_->changed().connect([this] {
        file_upload_->upload();
        status_.setText("File upload is changed.");
    });

    file_upload_->uploaded().connect([this] {
        error_ = ErrorType::None;
        is_valid_ = true;
        status_.setText("File upload is finished. Confirm changes");
    });

    file_upload_->fileTooLarge().connect([=] {
        error_ = ErrorType::ImgTooLarge;
        is_valid_ = false;
        status_.setText("File is too large (over 4 MB). Try again");
        create_file_uploader();
    });
}

void EditProfileForm::change_avatar() {
    if (is_valid_) {
        auto avatar_path = copy_temp_img_to_avatar_folder(
                file_upload_->spoolFileName(), file_upload_->clientFileName().toUTF8());
        auto avatarLink = server_.update_avatar(id_, avatar_path);
        updated_avatar_.emit(avatarLink);
    }
}

std::string
EditProfileForm::copy_temp_img_to_avatar_folder(
        const std::string &tmp_file_path, const std::string &client_filename)
{
    auto ext = std::filesystem::path(client_filename).extension();

    std::stringstream time;

    time << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string result_filename = "./avatars/" + time.str() + ext.string();
    std::ifstream in(tmp_file_path, std::ios::binary | std::ios::in);
    std::ofstream out(result_filename, std::ios::binary | std::ios::out);

    char byte;
    while (in.read(&byte, sizeof(char))) {
        out.write(&byte, sizeof(char));
    }
    in.close();
    out.close();

    return result_filename;
}
