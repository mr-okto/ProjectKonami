#ifndef PROJECTKONAMI_REGISTRATIONFORM_HPP
#define PROJECTKONAMI_REGISTRATIONFORM_HPP

#include <Wt/WContainerWidget.h>

struct RegistrationForm : public Wt::WContainerWidget {
public:
    RegistrationForm();

private:
    void create_UI();

    Wt::WLineEdit *username_edit_;
    Wt::WLineEdit *password_edit_first_;
    Wt::WLineEdit *password_edit_second_;
};


#endif //PROJECTKONAMI_REGISTRATIONFORM_HPP
